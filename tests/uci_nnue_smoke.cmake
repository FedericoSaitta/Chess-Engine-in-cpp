if(NOT DEFINED ENGINE OR NOT DEFINED INPUT)
    message(FATAL_ERROR "ENGINE and INPUT are required")
endif()

execute_process(
    COMMAND "${ENGINE}"
    INPUT_FILE "${INPUT}"
    OUTPUT_VARIABLE output
    ERROR_VARIABLE errors
    RESULT_VARIABLE result
    TIMEOUT 30
)

if(NOT result EQUAL 0)
    message(FATAL_ERROR "Aramis exited with ${result}\nstdout:\n${output}\nstderr:\n${errors}")
endif()

foreach(required
        "option name UseNNUE type check default true"
        "option name EvalFile type string default pesos_bullet_512_buckets8.bin"
        "uciok"
        "NNUE load failed"
        "keeping"
        "readyok"
        "bestmove ")
    string(FIND "${output}" "${required}" found)
    if(found EQUAL -1)
        message(FATAL_ERROR "Missing '${required}' in UCI output:\n${output}")
    endif()
endforeach()

if(output MATCHES "bestmove 0000")
    message(FATAL_ERROR "UCI returned a null bestmove after NNUE fallback:\n${output}")
endif()
