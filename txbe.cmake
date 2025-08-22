function(add_txbe_backend NAME SRC LIBS INCLUDES)
  add_library(txbe_backend_${NAME} INTERFACE)

  target_include_directories(txbe_backend_${NAME}
                             INTERFACE
                             ${INCLUDES})

  target_link_libraries(txbe_backend_${NAME}
                        INTERFACE
                        ${LIBS})

  set_property(TARGET
               txbe_backend_${NAME}
               PROPERTY
               BACKEND_SOURCES
               "${SRC}")
endfunction()