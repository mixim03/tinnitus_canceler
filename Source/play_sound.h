
#pragma once

#include <JuceHeader.h>

//==============================================================================
class AudioAppDemo : public juce::AudioAppComponent
{
private:
    int mCH;

    juce::OwnedArray<juce::Slider> slider;

    int mVal[3];

    juce::ToggleButton button;

public:
    enum {
        NUM_OF_WAVE = 8,
        NUM_OF_WAVE_W = 2,
        NUM_OF_WAVE_H = NUM_OF_WAVE / NUM_OF_WAVE_W
    };

    enum {
        CH_L,
        CH_R,
        NUM_OF_CHANNEL,
    };
    
    enum {
        SLIDER_FREQ,
        SLIDER_LEVEL,
        SLIDER_PHASE,
        NUM_OF_SLIDER
    };

    static int getCH(int theIdx) {
        if (theIdx < (NUM_OF_WAVE_H)) {
            return CH_L;
        }
        return CH_R;
    }
    //==============================================================================
    AudioAppDemo(int theCH, int theIdx)
#ifdef JUCE_DEMO_RUNNER
        : AudioAppComponent(getSharedAudioDeviceManager(0, 2))
#endif
    {
        addAndMakeVisible(button);
//        button.setColour(juce::TextButton::textColourOffId, juce::Colours::darkgrey);
//        button.setColour(juce::TextButton::textColourOnId, juce::Colours::red);
        button.setButtonText("Sw");

        //Slider
        for (int i = 0; i < NUM_OF_SLIDER; i++) {
            slider.add(new juce::Slider("test"));
            addAndMakeVisible(slider[i]);
        }

        slider[SLIDER_FREQ]->setRange(0, 20000, 0.1);
        slider[SLIDER_LEVEL]->setRange(0, 0.1, 0.001);
        slider[SLIDER_PHASE]->setRange(0, 360, 0.1);

        slider[SLIDER_FREQ]->onValueChange = [this] { 
            mFrequency = slider[SLIDER_FREQ]->getValue(); 
            mPhaseDelta = (float)(juce::MathConstants<double>::twoPi * mFrequency / sampleRate);
        };

        slider[SLIDER_LEVEL]->onValueChange = [this] { mAmplitude = slider[SLIDER_LEVEL]->getValue(); };
        slider[SLIDER_PHASE]->onValueChange = [this] { mPhaseOfst = slider[SLIDER_PHASE]->getValue(); };

        //Audio
        setAudioChannels(0, NUM_OF_CHANNEL);
        mCH = theCH;

        //setSize(800, 600/4);
        int aH = 600 / NUM_OF_WAVE_H;

        if (getCH(theIdx)==CH_L) {
            setBounds(0,   aH * theIdx, 400, 600 / NUM_OF_WAVE_H);
        }
        else {
            setBounds(400, aH * (theIdx-4), 400, 600 / NUM_OF_WAVE_H);
        }

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
        int aSw = button.getToggleStateValue().getValue();      
        if (aSw == 0) {
            return;
        }

        bufferToFill.clearActiveBufferRegion();
        auto originalPhase = mPhase;

        //for (auto chan = 0; chan < bufferToFill.buffer->getNumChannels(); ++chan)
        //{
            mPhase = originalPhase;

            auto* channelData = bufferToFill.buffer->getWritePointer(mCH, bufferToFill.startSample);

            for (auto i = 0; i < bufferToFill.numSamples; ++i)
            {
                channelData[i] = mAmplitude * std::sin(mPhaseOfst + mPhase);

                // increment the phase step for the next sample
                mPhase = std::fmod(mPhase + mPhaseDelta, juce::MathConstants<float>::twoPi);
            }
        //}
    }

    void releaseResources() override
    {
        // This gets automatically called when audio device parameters change
        // or device is restarted.
    }


    //==============================================================================
    void paint(juce::Graphics& g) override
    {
#if 0

        // (Our component is opaque, so we must completely fill the background with a solid colour)
        g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

        auto centreY = (float)getHeight() / 2.0f;
        auto radius = mAmplitude * 200.0f;

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
            wavePath.lineTo(x, centreY + mAmplitude * (float)getHeight() * 2.0f
                * std::sin(x * mFrequency * 0.0001f));

        g.setColour(getLookAndFeel().findColour(juce::Slider::thumbColourId));
        g.strokePath(wavePath, juce::PathStrokeType(2.0f));
#endif
        auto aArea = getLocalBounds();
        g.setColour(juce::Colours::white);
        //g.fillAll(juce::Colours::black);
        g.drawRect(aArea);
    }

#if 0
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
#endif

    void resized() override
    {
        // This is called when the component is resized.
        // If you add any child components, this is where you should
        // update their positions.
   
        auto aAreaa = getLocalBounds();
        button.setBounds(5, aAreaa.getHeight()/2-20, 40, 40);

        auto aAreaaReduced = aAreaa.reduced(50, 10);
        int  aItemH = aAreaaReduced.getHeight() / 3;


       juce::Array<juce::Rectangle<int>> aSliderArea;

       for (int i = 0; i < 3; i++) {
           aSliderArea.add(aAreaaReduced.removeFromTop(aItemH));
           slider[i]->setBounds(aSliderArea[i]);
       }
    }


private:
    //==============================================================================
    float mPhase      = 0.0f;
    float mPhaseDelta = 0.0f;
    float mFrequency  = 5000.0f;
    float mAmplitude  = 0.0f;
    float mPhaseOfst  = 0.0f;

    double sampleRate = 0.0;
    int expectedSamplesPerBlock = 0;
    juce::Point<float> lastMousePosition;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioAppDemo)
};
