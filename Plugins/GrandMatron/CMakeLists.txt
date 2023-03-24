project(GrandMatron VERSION 1.0.9)

set (BaseTargetName GrandMatron)

juce_add_plugin("${BaseTargetName}"
        # VERSION ...                               # Set this if the plugin version is different to the project version
        # ICON_BIG ...                              # ICON_* arguments specify a path to an image file to use as an icon for the Standalone
        # ICON_SMALL ...
        COMPANY_NAME "dadsquad"
        IS_SYNTH FALSE
        NEEDS_MIDI_INPUT TRUE
        NEEDS_MIDI_OUTPUT FALSE
        IS_MIDI_EFFECT FALSE
        EDITOR_WANTS_KEYBOARD_FOCUS FALSE
        COPY_PLUGIN_AFTER_BUILD TRUE
        PLUGIN_MANUFACTURER_CODE Juce
        PLUGIN_CODE GMxt
        FORMATS AU VST3 Standalone
        PRODUCT_NAME "Grand Matron")

target_sources(${BaseTargetName} PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
        Source/EmbeddedFonts.cpp
        Source/MatronLookAndFeel.cpp
        Source/MatronHeaderUI.cpp
        Source/MatronFooterUI.cpp)

target_compile_definitions(${BaseTargetName}
        PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0)

juce_add_binary_data(${PROJECT_NAME}_data
        SOURCES
        Resources/ticks.svg
        Resources/matronGearDot.svg
        Resources/decoration.svg
        Resources/footerDecorationL.svg
        Resources/footerDecorationR.svg
        Resources/weights/neural_net_weights.json
        Resources/CutiveRegular.ttf
        Resources/MontserratBlack900.ttf
        #     Resources/MontserratBlackItalic900.ttf
        Resources/MontserratBold700.ttf
        #     Resources/MontserratBoldItalic700.ttf
        #     Resources/MontserratExtraBoldItalic800.ttf
        #     Resources/MontserratExtraBold800.ttf
        #     Resources/MontserratExtraLightItalic275.ttf
        #     Resources/MontserratExtraLight275.ttf
        #     Resources/MontserratItalic400.ttf
        #     Resources/MontserratLightItalic300.ttf
        Resources/MontserratLight300.ttf
        #     Resources/MontserratMediumItalic500.ttf
        #     Resources/MontserratMedium500.ttf
        Resources/MontserratRegular400.ttf
        #     Resources/MontserratSemiBoldItalic600.ttf
        #     Resources/MontserratSemiBold600.ttf
        #     Resources/MontserratThin250.ttf
        #     Resources/MontserratLightItalic300.ttf
        )

target_link_libraries(${BaseTargetName} PRIVATE
        shared_plugin_helpers
        GrandMatron_data
        juce_audio_utils
        juce_dsp
        juce_recommended_config_flags
        juce_recommended_lto_flags
        juce_recommended_warning_flags)

target_link_libraries(${BaseTargetName} PUBLIC RTNeural)
