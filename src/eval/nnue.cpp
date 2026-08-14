#include "nnue.h"

#include "board.h"
#include "inline_functions.h"
#include "misc.h"
#include "move.h"
#include "types.h"

#include <algorithm>
#include <bit>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <ostream>
#include <string_view>
#include <vector>

namespace nnue {
namespace {

constexpr int QA = 255;
constexpr int QB = 64;
constexpr int OUTPUT_SCALE = 400;
constexpr std::size_t USEFUL_BYTES =
    (INPUTS * HIDDEN + HIDDEN + OUTPUT_BUCKETS * 2 * HIDDEN + OUTPUT_BUCKETS) * 2;
constexpr std::size_t FILE_BYTES = 803'904;
constexpr std::string_view PADDING_SIGNATURE = "bullet";

struct Network {
    std::vector<std::int16_t> inputWeights;
    std::array<std::int16_t, HIDDEN> inputBiases{};
    std::array<std::int16_t, OUTPUT_BUCKETS * 2 * HIDDEN> outputWeights{};
    std::array<std::int16_t, OUTPUT_BUCKETS> outputBiases{};
    std::string path;
    std::uint64_t checksum{};
    bool loaded{};
};

Network network;

std::uint64_t fnv1a(const std::vector<std::uint8_t>& bytes) {
    std::uint64_t hash = 0xcbf29ce484222325ULL;
    for (const std::uint8_t byte : bytes) {
        hash ^= byte;
        hash *= 0x100000001b3ULL;
    }
    return hash;
}

std::int16_t readI16(const std::vector<std::uint8_t>& bytes, std::size_t& cursor) {
    const std::uint16_t raw = static_cast<std::uint16_t>(bytes[cursor])
                            | (static_cast<std::uint16_t>(bytes[cursor + 1]) << 8);
    cursor += 2;
    return std::bit_cast<std::int16_t>(raw);
}

std::int16_t wrappingAdd(const std::int16_t lhs, const std::int16_t rhs) {
    const auto raw = static_cast<std::uint16_t>(lhs) + static_cast<std::uint16_t>(rhs);
    return std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(raw));
}

std::int16_t wrappingSub(const std::int16_t lhs, const std::int16_t rhs) {
    const auto raw = static_cast<std::uint16_t>(lhs) - static_cast<std::uint16_t>(rhs);
    return std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(raw));
}

std::size_t featureIndex(const int perspective, const int piece, const int square) {
    const int pieceColor = color(static_cast<Piece>(piece));
    const int pieceKind = pieceType(static_cast<Piece>(piece));
    const int ownership = pieceColor == perspective ? 0 : 384;
    const int orientedSquare = perspective == WHITE ? square : square ^ 56;
    return static_cast<std::size_t>(ownership + pieceKind * 64 + orientedSquare);
}

bool addChange(std::array<FeatureChange, MAX_CHANGES>& changes,
               std::uint8_t& count,
               const Piece piece,
               const int square) {
    if (piece == NO_PIECE || square < 0 || square >= 64 || count >= MAX_CHANGES) return false;
    changes[count++] = FeatureChange{static_cast<std::uint8_t>(piece),
                                     static_cast<std::uint8_t>(square)};
    return true;
}

bool checkOneMove(const std::string& fen,
                  const std::string& moveText,
                  std::ostream& output) {
    Board board(fen);
    Accumulator incremental;
    if (!incremental.refresh(board)) return false;
    const Accumulator root = incremental;

    const Move move = parseMove(moveText, board);
    if (move.isNone()) {
        output << "NNUE self-test: could not parse " << moveText << '\n';
        return false;
    }
    const Update update = incremental.prepareUpdate(board, move);
    if (!update.valid || !board.makeMove(move, 0)) {
        output << "NNUE self-test: move rejected " << moveText << '\n';
        return false;
    }
    incremental.apply(update, board.side);

    Accumulator refreshed;
    if (!refreshed.refresh(board) || !incremental.sameState(refreshed)
        || incremental.evaluate() != refreshed.evaluate()) {
        output << "NNUE self-test: incremental mismatch after " << moveText << '\n';
        return false;
    }

    board.undo(move);
    incremental.undo(update, board.side);
    if (!incremental.sameState(root) || incremental.evaluate() != root.evaluate()) {
        output << "NNUE self-test: undo mismatch after " << moveText << '\n';
        return false;
    }
    return true;
}

} // namespace

bool loadNetwork(const std::string& path, std::string& error) {
    std::ifstream input(path, std::ios::binary);
    if (!input) {
        error = "could not open " + path;
        return false;
    }

    const std::vector<std::uint8_t> bytes((std::istreambuf_iterator<char>(input)),
                                          std::istreambuf_iterator<char>());
    if (bytes.size() != FILE_BYTES) {
        error = "unexpected NNUE size " + std::to_string(bytes.size())
              + " (expected 803904 bytes for 768x512, 8 buckets)";
        return false;
    }
    static_assert(USEFUL_BYTES == 803'856);
    for (std::size_t i = USEFUL_BYTES; i < bytes.size(); ++i) {
        if (bytes[i] != static_cast<std::uint8_t>(PADDING_SIGNATURE[(i - USEFUL_BYTES)
                                                                  % PADDING_SIGNATURE.size()])) {
            error = "invalid Bullet padding signature";
            return false;
        }
    }

    Network candidate;
    candidate.inputWeights.resize(INPUTS * HIDDEN);
    std::size_t cursor = 0;
    for (auto& weight : candidate.inputWeights) weight = readI16(bytes, cursor);
    for (auto& bias : candidate.inputBiases) bias = readI16(bytes, cursor);
    for (auto& weight : candidate.outputWeights) weight = readI16(bytes, cursor);
    for (auto& bias : candidate.outputBiases) bias = readI16(bytes, cursor);
    if (cursor != USEFUL_BYTES) {
        error = "internal NNUE layout mismatch";
        return false;
    }

    candidate.path = path;
    candidate.checksum = fnv1a(bytes);
    candidate.loaded = true;
    network = std::move(candidate);
    error.clear();
    return true;
}

bool loadDefaultNetwork(const std::string& executablePath, std::string& error) {
    namespace fs = std::filesystem;
    std::vector<fs::path> candidates;
    if (!executablePath.empty()) {
        std::error_code ec;
        const fs::path executable = fs::absolute(executablePath, ec);
        if (!ec) candidates.push_back(executable.parent_path() / DEFAULT_FILE);
    }
    candidates.emplace_back(fs::current_path() / DEFAULT_FILE);
    candidates.emplace_back(fs::current_path() / "networks" / DEFAULT_FILE);

    std::string lastError;
    for (const auto& candidate : candidates) {
        if (loadNetwork(candidate.string(), lastError)) return true;
    }
    error = lastError.empty() ? "default NNUE file was not found" : lastError;
    return false;
}

bool networkAvailable() {
    return network.loaded;
}

std::string loadedNetworkPath() {
    return network.path;
}

std::uint64_t loadedNetworkChecksum() {
    return network.checksum;
}

void Accumulator::addFeature(const int perspective, const int piece, const int square) {
    const std::size_t feature = featureIndex(perspective, piece, square);
    const std::size_t base = feature * HIDDEN;
    for (std::size_t neuron = 0; neuron < HIDDEN; ++neuron) {
        values_[perspective][neuron] =
            wrappingAdd(values_[perspective][neuron], network.inputWeights[base + neuron]);
    }
}

void Accumulator::removeFeature(const int perspective, const int piece, const int square) {
    const std::size_t feature = featureIndex(perspective, piece, square);
    const std::size_t base = feature * HIDDEN;
    for (std::size_t neuron = 0; neuron < HIDDEN; ++neuron) {
        values_[perspective][neuron] =
            wrappingSub(values_[perspective][neuron], network.inputWeights[base + neuron]);
    }
}

bool Accumulator::refresh(const Board& board) {
    if (!network.loaded) {
        valid_ = false;
        return false;
    }
    values_[WHITE] = network.inputBiases;
    values_[BLACK] = network.inputBiases;
    pieceCount_ = 0;
    for (int square = 0; square < 64; ++square) {
        const Piece piece = board.mailbox[square];
        if (piece == NO_PIECE) continue;
        ++pieceCount_;
        addFeature(WHITE, piece, square);
        addFeature(BLACK, piece, square);
    }
    sideToMove_ = board.side;
    valid_ = true;
    return true;
}

Update Accumulator::prepareUpdate(const Board& board, const Move& move) const {
    Update update;
    if (!valid_ || move.isNone()) return update;

    const Piece moving = board.mailbox[move.from()];
    if (moving == NO_PIECE || color(moving) != board.side) return update;
    if (!addChange(update.removed, update.removedCount, moving, move.from())) return update;

    if (move.isCastling()) {
        const bool kingSide = move.to() > move.from();
        const int rankBase = move.from() & ~7;
        const int rookFrom = rankBase + (kingSide ? 7 : 0);
        const int rookTo = rankBase + (kingSide ? 5 : 3);
        const Piece rook = board.mailbox[rookFrom];
        if (!addChange(update.removed, update.removedCount, rook, rookFrom)
            || !addChange(update.added, update.addedCount, moving, move.to())
            || !addChange(update.added, update.addedCount, rook, rookTo)) return Update{};
    } else if (move.isEnPassant()) {
        const int capturedSquare = move.to() + (board.side == WHITE ? -8 : 8);
        const Piece captured = board.mailbox[capturedSquare];
        if (!addChange(update.removed, update.removedCount, captured, capturedSquare)
            || !addChange(update.added, update.addedCount, moving, move.to())) return Update{};
    } else if (move.isPromotion()) {
        if (move.isCapture()) {
            const Piece captured = board.mailbox[move.to()];
            if (!addChange(update.removed, update.removedCount, captured, move.to())) return Update{};
        }
        const Piece promoted = make_piece(static_cast<Color>(board.side), move.promotionPiece());
        if (!addChange(update.added, update.addedCount, promoted, move.to())) return Update{};
    } else {
        if (move.isCapture()) {
            const Piece captured = board.mailbox[move.to()];
            if (!addChange(update.removed, update.removedCount, captured, move.to())) return Update{};
        }
        if (!addChange(update.added, update.addedCount, moving, move.to())) return Update{};
    }

    update.valid = true;
    return update;
}

void Accumulator::apply(const Update& update, const int sideToMove) {
    if (!valid_ || !update.valid) return;
    for (int perspective = WHITE; perspective <= BLACK; ++perspective) {
        for (std::size_t i = 0; i < update.removedCount; ++i) {
            removeFeature(perspective, update.removed[i].piece, update.removed[i].square);
        }
        for (std::size_t i = 0; i < update.addedCount; ++i) {
            addFeature(perspective, update.added[i].piece, update.added[i].square);
        }
    }
    pieceCount_ += static_cast<int>(update.addedCount) - static_cast<int>(update.removedCount);
    sideToMove_ = sideToMove;
}

void Accumulator::undo(const Update& update, const int sideToMove) {
    if (!valid_ || !update.valid) return;
    for (int perspective = WHITE; perspective <= BLACK; ++perspective) {
        for (std::size_t i = 0; i < update.addedCount; ++i) {
            removeFeature(perspective, update.added[i].piece, update.added[i].square);
        }
        for (std::size_t i = 0; i < update.removedCount; ++i) {
            addFeature(perspective, update.removed[i].piece, update.removed[i].square);
        }
    }
    pieceCount_ += static_cast<int>(update.removedCount) - static_cast<int>(update.addedCount);
    sideToMove_ = sideToMove;
}

void Accumulator::setSideToMove(const int sideToMove) {
    sideToMove_ = sideToMove;
}

int Accumulator::evaluate() const {
    if (!valid_ || !network.loaded) return 0;
    const int us = sideToMove_;
    const int them = sideToMove_ ^ 1;
    const std::size_t bucket = std::min(OUTPUT_BUCKETS - 1,
        static_cast<std::size_t>(std::max(pieceCount_, 2) - 2) / 4);
    const std::size_t outputBase = bucket * 2 * HIDDEN;
    std::int64_t sum = 0;
    for (std::size_t neuron = 0; neuron < HIDDEN; ++neuron) {
        const std::int64_t ours = std::clamp<int>(values_[us][neuron], 0, QA);
        const std::int64_t theirs = std::clamp<int>(values_[them][neuron], 0, QA);
        sum += ours * ours * network.outputWeights[outputBase + neuron];
        sum += theirs * theirs * network.outputWeights[outputBase + HIDDEN + neuron];
    }
    const std::int64_t quantized = sum / QA + network.outputBiases[bucket];
    return static_cast<int>(std::lround(static_cast<double>(quantized * OUTPUT_SCALE)
                                        / static_cast<double>(QA * QB)));
}

bool Accumulator::sameState(const Accumulator& other) const {
    return valid_ == other.valid_ && sideToMove_ == other.sideToMove_
        && pieceCount_ == other.pieceCount_ && values_ == other.values_;
}

bool runSelfTests(const std::string& weightsPath, std::ostream& output) {
    std::string error;
    if (!loadNetwork(weightsPath, error)) {
        output << "NNUE self-test: load failed: " << error << '\n';
        return false;
    }
    if (loadedNetworkChecksum() == 0) {
        output << "NNUE self-test: invalid zero checksum\n";
        return false;
    }

    // Independent reference value for the exact production file. This also
    // catches a wrong square orientation, perspective order or output bucket,
    // errors that incremental-vs-refresh alone would reproduce on both sides.
    Board referenceBoard("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    Accumulator referenceAccumulator;
    if (!referenceAccumulator.refresh(referenceBoard) || referenceAccumulator.evaluate() != 16) {
        output << "NNUE self-test: start position reference mismatch (expected 16 cp)\n";
        return false;
    }

    const struct {
        const char* fen;
        const char* move;
    } specialMoves[] = {
        {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", "e2e4"},
        {"4k3/8/8/3p4/4P3/8/8/4K3 w - - 0 1", "e4d5"},
        {"4k3/8/8/3pP3/8/8/8/4K3 w - d6 0 1", "e5d6"},
        {"4k3/8/8/8/8/8/8/R3K2R w KQ - 0 1", "e1g1"},
        {"4k3/P7/8/8/8/8/8/4K3 w - - 0 1", "a7a8q"},
        {"1r2k3/P7/8/8/8/8/8/4K3 w - - 0 1", "a7b8q"},
    };
    for (const auto& test : specialMoves) {
        if (!checkOneMove(test.fen, test.move, output)) return false;
    }

    Board board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    Accumulator incremental;
    if (!incremental.refresh(board)) return false;
    std::uint64_t random = 0x9e3779b97f4a7c15ULL;
    int checked = 0;
    for (int ply = 0; ply < 256; ++ply) {
        MoveList moves;
        board.generateMoves(moves);
        if (moves.count == 0) break;
        random ^= random << 7;
        random ^= random >> 9;
        bool made = false;
        for (int offset = 0; offset < moves.count; ++offset) {
            const int index = (static_cast<int>(random % moves.count) + offset) % moves.count;
            const Move move = moves.moves[index].first;
            const Update update = incremental.prepareUpdate(board, move);
            if (!board.makeMove(move, 0)) continue;
            incremental.apply(update, board.side);
            Accumulator refreshed;
            if (!refreshed.refresh(board) || !incremental.sameState(refreshed)
                || incremental.evaluate() != refreshed.evaluate()) {
                output << "NNUE self-test: random incremental mismatch at ply " << ply << '\n';
                return false;
            }
            ++checked;
            made = true;
            break;
        }
        if (!made) break;
    }
    if (checked < 64) {
        output << "NNUE self-test: only " << checked << " random positions checked\n";
        return false;
    }

    const std::string pathBefore = loadedNetworkPath();
    const std::uint64_t checksumBefore = loadedNetworkChecksum();
    const int evaluationBefore = incremental.evaluate();
    if (loadNetwork(weightsPath + ".missing", error) || !networkAvailable()
        || loadedNetworkPath() != pathBefore || loadedNetworkChecksum() != checksumBefore
        || incremental.evaluate() != evaluationBefore) {
        output << "NNUE self-test: failed load did not preserve the active network\n";
        return false;
    }

    output << "NNUE self-test: PASS; format=768x512x2, buckets=8, useful=803856, "
           << "padding=48, checksum=" << std::hex << checksumBefore << std::dec
           << ", random_positions=" << checked << '\n';
    return true;
}

} // namespace nnue
