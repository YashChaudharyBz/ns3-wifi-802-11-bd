check_include_file_cxx(stdint.h HAVE_STDINT_H)
if(HAVE_STDINT_H)
    add_definitions(-DHAVE_STDINT_H)
endif()

set(examples_as_tests_sources)
if(${ENABLE_EXAMPLES})
    set(examples_as_tests_sources
        #test/wave-bd-examples-test-suite.cc
        )
endif()

build_lib(
    LIBNAME wave-bd
    SOURCE_FILES helper/wave-bd-mac-helper.cc
                 helper/wifi-80211bd-helper.cc
                 helper/yans-wifi-bd-helper.cc
                 model/bd-rep-manager.cc
                 model/bd-tag.cc
                 model/ocb-wifi-mac-bd.cc
                 model/wave-bd-frame-exchange-manager.cc
                 model/yans-wifi-phy-bd.cc

    HEADER_FILES helper/wave-bd-mac-helper.h
                 helper/wifi-80211bd-helper.h
                 helper/yans-wifi-bd-helper.h
                 model/bd-rep-manager.h
                 model/bd-tag.h
                 model/ocb-wifi-mac-bd.h
                 model/wave-bd-frame-exchange-manager.h
                 model/yans-wifi-phy-bd.h
    LIBRARIES_TO_LINK ${libcore} 
                      ${libwifi} 
                      ${libmobility} 
                      ${libinternet}
                      ${libspectrum}
    TEST_SOURCES test/wave-bd-test-suite.cc
                 ${examples_as_tests_sources}
)

