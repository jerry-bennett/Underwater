/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/
#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
UnderwaterAudioProcessor::UnderwaterAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),state(*this,nullptr,Identifier("PitchParameters"),createParameterLayout()),
                        treeState(*this, nullptr, ProjectInfo::projectName, createParameterLayout())
#endif
{
}

UnderwaterAudioProcessor::~UnderwaterAudioProcessor()
{
}

AudioProcessorValueTreeState::ParameterLayout UnderwaterAudioProcessor::createParameterLayout(){
    
    std::vector<std::unique_ptr<RangedAudioParameter>> params;
    
    params.push_back(std::make_unique<AudioParameterFloat>("PITCH","Pitch",-12.f,12.f,0.f));
    
    NormalisableRange<float> gainRange(0.f,1.f,.01f);
    //gainRange.setSkewForCentre(0.2f);
    params.push_back(std::make_unique<AudioParameterFloat>("GAIN","Gain",gainRange,1.f));
    
    return {params.begin() , params.end()};
}

//==============================================================================
const juce::String UnderwaterAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool UnderwaterAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool UnderwaterAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool UnderwaterAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double UnderwaterAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int UnderwaterAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int UnderwaterAudioProcessor::getCurrentProgram()
{
    return 0;
}

void UnderwaterAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String UnderwaterAudioProcessor::getProgramName (int index)
{
    return {};
}

void UnderwaterAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void UnderwaterAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    
}

void UnderwaterAudioProcessor::releaseResources()
{
    
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool UnderwaterAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void UnderwaterAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());


    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        for (int n = 0; n < buffer.getNumSamples() ; ++n){
            
            const int range = abs(4);
            long start = 0;
        
            struct timespec ts;
            clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
            long now = ts.tv_sec * 1000 + ts.tv_nsec / 100000ULL;//two zeros
            float ellapsed = (now - start) / 50.0f;
            if (start == 0) { start = now; }
            
            pitchTemp = sin(ellapsed/range) * range;
            pitchShifter.setPitch(pitchTemp);
            
            float x = buffer.getReadPointer(channel)[n];
            float y = pitchShifter.processSample(x, channel);
            float out = (0.25 * x) + (0.75 * y); //* gainSmooth;
            buffer.getWritePointer(channel)[n] = out;
            // Metering
            //outValue[channel] = vuAnalysis.processSample(out, channel);
            
        }
    }
}

//==============================================================================
bool UnderwaterAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* UnderwaterAudioProcessor::createEditor()
{
    return new UnderwaterAudioProcessorEditor (*this);
}

//==============================================================================
void UnderwaterAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto currentState = state.copyState();
    std::unique_ptr<XmlElement> xml(currentState.createXml());
    copyXmlToBinary(*xml, destData);
}

void UnderwaterAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if(xmlState &&xmlState->hasTagName(state.state.getType())){
        state.replaceState(ValueTree::fromXml(*xmlState));
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new UnderwaterAudioProcessor();
}
