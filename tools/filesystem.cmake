macro(ListDirectories result current)
    file(GLOB items LIST_DIRECTORIES true RELATIVE ${current} "${current}/*")
    set(dirs "")

    foreach(item ${items})
        if(IS_DIRECTORY ${current}/${item})
            list(APPEND dirs ${item})
        endif()
    endforeach()

    set(${result} ${dirs})
    set(${dirs} "")
    set(${items} "")
endmacro()
