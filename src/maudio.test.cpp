#include <catch2/catch.hpp>

#include "maudio/maudio_utils.h"

using namespace MAudio;

TEST_CASE("Audio.NoteConversion", "[Audio]")
{
    double midi = note_string_to_midi("c#4");
    REQUIRE((int)midi == (int)61);
   
    double midi_same = note_string_to_midi("db4");
    REQUIRE((int)midi == (int)midi_same);

    midi = note_string_to_midi("ex4");
    REQUIRE((int)midi == (int)66);
    
    midi = note_string_to_midi("abb6");
    REQUIRE(midi == (int)91);

    /*
    midi = note_string_to_midi("ab4");
    ASSERT_DOUBLE_EQ(midi ,(int)91);
    */

    // Invalid
    REQUIRE((int)note_string_to_midi("4c") == (int)60);
}

TEST_CASE("Audio.Frequency", "[Audio]")
{
    auto midi = note_string_to_midi("A4");
    REQUIRE((int)midi_to_frequency(midi) == 440);
    
    midi = note_string_to_midi("Cb6");
    //REQUIRE_THAT(midi_to_frequency(midi), Catch::NonCopyable987.7666, .0001);
}
