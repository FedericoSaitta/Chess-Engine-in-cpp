#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <iosfwd>
#include <string>

class Board;
class Move;

namespace nnue {

inline constexpr std::size_t INPUTS = 768;
inline constexpr std::size_t HIDDEN = 512;
inline constexpr std::size_t OUTPUT_BUCKETS = 8;
inline constexpr std::size_t MAX_CHANGES = 4;
inline constexpr const char* DEFAULT_FILE = "pesos_bullet_512_buckets8.bin";

struct FeatureChange {
    std::uint8_t piece{};
    std::uint8_t square{};
};

struct Update {
    std::array<FeatureChange, MAX_CHANGES> removed{};
    std::array<FeatureChange, MAX_CHANGES> added{};
    std::uint8_t removedCount{};
    std::uint8_t addedCount{};
    bool valid{};
};

// Loading is transactional: a bad path or invalid file leaves the currently
// loaded network untouched. If no valid network exists, callers safely fall
// back to Aramis' handcrafted evaluation.
bool loadNetwork(const std::string& path, std::string& error);
bool loadDefaultNetwork(const std::string& executablePath, std::string& error);
bool networkAvailable();
std::string loadedNetworkPath();
std::uint64_t loadedNetworkChecksum();

class Accumulator {
public:
    bool refresh(const Board& board);
    Update prepareUpdate(const Board& board, const Move& move) const;
    void apply(const Update& update, int sideToMove);
    void undo(const Update& update, int sideToMove);
    void setSideToMove(int sideToMove);

    [[nodiscard]] bool valid() const { return valid_; }
    [[nodiscard]] int evaluate() const;
    [[nodiscard]] bool sameState(const Accumulator& other) const;

private:
    std::array<std::array<std::int16_t, HIDDEN>, 2> values_{};
    int sideToMove_{};
    int pieceCount_{};
    bool valid_{};

    void addFeature(int perspective, int piece, int square);
    void removeFeature(int perspective, int piece, int square);
};

bool runSelfTests(const std::string& weightsPath, std::ostream& output);

} // namespace nnue
