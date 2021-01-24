
#pragma once

#include <JuceHeader.h>

//==============================================================================
class AudioAppDemo : public juce::AudioAppComponent
{
public:
    //==============================================================================
    AudioAppDemo()
#ifdef JUCE_DEMO_RUNNER
        : AudioAppComponent(getSharedAudioDeviceManager(0, 2))
#endif
    {
        setAudioChannels(0, 2);

        setSize(800, 600);
    }

    ~AudioAppDemo() override
    {
        shutdownAudio();
    }

    //==============================================================================
    void prepareToPlay(int samplesPerBlockExpected, double newSampleRate) override
    {
        sampleRate = newSampleRate;
        expectedSamplesPerBlock = samplesPerBlockExpected;
    }

    /*  This method generates the actual audio samples.
        In this example the buffer is filled with a sine wave whose frequency and
        amplitude are controlled by the mouse position.
     */
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override
    {
        bufferToFill.clearActiveBufferRegion();
        auto originalPhase = phase;

        for (auto chan = 0; chan < bufferToFill.buffer->getNumChannels(); ++chan)
        {
            phase = originalPhase;

            auto* channelData = bufferToFill.buffer->getWritePointer(chan, bufferToFill.startSample);

            for (auto i = 0; i < bufferToFill.numSamples; ++i)
            {
                channelData[i] = amplitude * std::sin(phase);

                // increment the phase step for the next sample
                phase = std::fmod(phase + phaseDelta, juce::MathConstants<float>::twoPi);
            }
        }
    }

    void releaseResources() override
    {
        // This gets automatically called when audio device parameters change
        // or device is restarted.
    }


    //==============================================================================
    void paint(juce::Graphics& g) override
    {
        // (Our component is opaque, so we must completely fill the background with a solid colour)
        g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

        auto centreY = (float)getHeight() / 2.0f;
        auto radius = amplitude * 200.0f;

        if (radius >= 0.0f)
        {
            // Draw an ellipse based on the mouse position and audio volume
            g.setColour(juce::Colours::lightgreen);

            g.fillEllipse(juce::jmax(0.0f, lastMousePosition.x) - radius / 2.0f,
                juce::jmax(0.0f, lastMousePosition.y) - radius / 2.0f,
                radius, radius);
        }

        // Draw a representative sine wave.
        juce::Path wavePath;
        wavePath.startNewSubPath(0, centreY);

        for (auto x = 1.0f; x < (float)getWidth(); ++x)
            wavePath.lineTo(x, centreY + amplitude * (float)getHeight() * 2.0f
                * std::sin(x * frequency * 0.0001f));

        g.setColour(getLookAndFeel().findColour(juce::Slider::thumbColourId));
        g.strokePath(wavePath, juce::PathStrokeType(2.0f));
    }

    // Mouse handling..
    void mouseDown(const juce::MouseEvent& e) override
    {
        mouseDrag(e);
    }

    void mouseDrag(const juce::MouseEvent& e) override
    {
        lastMousePosition = e.position;

        frequency = (float)(getHeight() - e.y) * 10.0f;
        amplitude = juce::jmin(0.9f, 0.2f * e.position.x / (float)getWidth());

        phaseDelta = (float)(juce::MathConstants<double>::twoPi * frequency / sampleRate);

        repaint();
    }

    void mouseUp(const juce::MouseEvent&) override
    {
        amplitude = 0.0f;
        repaint();
    }

    void resized() override
    {
        // This is called when the component is resized.
        // If you add any child components, this is where you should
        // update their positions.
    }


private:
    //==============================================================================
    float phase = 0.0f;
    float phaseDelta = 0.0f;
    float frequency = 5000.0f;
    float amplitude = 0.2f;

    double sampleRate = 0.0;
    int expectedSamplesPerBlock = 0;
    juce::Point<float> lastMousePosition;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioAppDemo)
};
