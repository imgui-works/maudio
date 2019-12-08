#include <regex>
#include <map>

//#include "wavetable/wave_table_utils.h"

namespace MAudio
{

namespace
{

std::map<std::string, uint32_t> noteToScaleIndex = {
    { "cbb", -2 },
    { "cb", -1 },
    { "c", 0 },
    { "c#", 1 },
    { "cx", 2 },
    { "dbb", 0 },
    { "db", 1 },
    { "d", 2 },
    { "d#", 3 },
    { "dx", 4 },
    { "ebb", 2 },
    { "eb", 3 },
    { "e", 4 },
    { "e#", 5 },
    { "ex", 6 },
    { "fbb", 3 },
    { "fb", 4 },
    { "f", 5 },
    { "f#", 6 },
    { "fx", 7 },
    { "gbb", 5 },
    { "gb", 6 },
    { "g", 7 },
    { "g#", 8 },
    { "gx", 9 },
    { "abb", 7 },
    { "ab", 8 },
    { "a", 9 },
    { "a#", 10 },
    { "ax", 11 },
    { "bbb", 9 },
    { "bb", 10 },
    { "b", 11 },
    { "b#", 12 },
    { "bx", 13 },
};

std::string scaleIndexToNote[] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };

} // namespace


void mutils_init()
{
}

float time_string_to_time(const std::string& str)
{
    return 0.0f;
}

float note_string_to_midi(const std::string& str)
{
    if (str.empty())
        return 0.0f;

    auto string_lower = [](const std::string& str)
    {
        std::string copy = str;
        std::transform(copy.begin(), copy.end(), copy.begin(), ::tolower);
        return copy;
    };

    auto strNote = string_lower(str);
    int octave = 4;
    auto index = strNote.find_first_not_of("abcdefg#x", 0);
    auto note = strNote.substr(0, index);
    if (index < strNote.size())
    {
        octave = std::stoi(strNote.substr(index));
    }

    auto itrFound = noteToScaleIndex.find(note);
    if (itrFound != noteToScaleIndex.end())
    {
        auto noteIndex = noteToScaleIndex[note];
        auto number = noteIndex + (octave + 1) * 12;
        return (float)number;
    }
    return 60.0f;
}


/**
 *  Transposes the frequency by the given number of semitones.
 *  @param  {Interval}  interval
 *  @return  {Tone.Frequency} A new transposed frequency
 *  @example
 * Tone.Frequency("A4").transpose(3); //"C5"
Tone.Frequency.prototype.transpose = function(interval){
    return new this.constructor(this.valueOf() * Tone.intervalToFrequencyRatio(interval));
};

/**
 *  Takes an array of semitone intervals and returns
 *  an array of frequencies transposed by those intervals.
 *  @param  {Array}  intervals
 *  @return  {Array<Tone.Frequency>} Returns an array of Frequencies
 *  @example
 * Tone.Frequency("A4").harmonize([0, 3, 7]); //["A4", "C5", "E5"]
Tone.Frequency.prototype.harmonize = function(intervals){
    return intervals.map(function(interval){
        return this.transpose(interval);
    }.bind(this));
};

///////////////////////////////////////////////////////////////////////////
//	UNIT CONVERSIONS
///////////////////////////////////////////////////////////////////////////

/**
 *  Return the value of the frequency as a MIDI note
 *  @return  {MIDI}
 *  @example
 * Tone.Frequency("C4").toMidi(); //60
Tone.Frequency.prototype.toMidi = function(){
    return Tone.Frequency.ftom(this.valueOf());
};

/**
 *  Return the value of the frequency in Scientific Pitch Notation
 *  @return  {Note}
 *  @example
 * Tone.Frequency(69, "midi").toNote(); //"A4"
Tone.Frequency.prototype.toNote = function(){
    var freq = this.toFrequency();
    var log = Math.log2(freq / Tone.Frequency.A4);
    var noteNumber = Math.round(12 * log) + 57;
    var octave = Math.floor(noteNumber/12);
    if (octave < 0){
        noteNumber += -12 * octave;
    }
    var noteName = scaleIndexToNote[noteNumber % 12];
    return noteName + octave.toString();
};

/**
 *  Return the duration of one cycle in seconds.
 *  @return  {Seconds}
Tone.Frequency.prototype.toSeconds = function(){
    return 1 / Tone.TimeBase.prototype.toSeconds.call(this);
};

/**
 *  Return the duration of one cycle in ticks
 *  @return  {Ticks}
Tone.Frequency.prototype.toTicks = function(){
    var quarterTime = this._beatsToUnits(1);
    var quarters = this.valueOf() / quarterTime;
    return Math.floor(quarters * Tone.Transport.PPQ);
};
*/

} // namespace MAudio
