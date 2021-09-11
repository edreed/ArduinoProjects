#include <array>
#include <utility>

namespace anime {
  using namespace std;
  
  class Configuration {
    public:
      static const uint32_t DEFAULT_STEP_MILLIS = 50ul;
      static const uint8_t  DEFAULT_BRIGHTNESS  = 32;

      void setStepMillis(uint32_t stepMillis) {
        _stepMillis = stepMillis;
      }

      uint32_t getStepMillis() const {
        return _stepMillis;
      }

      void setBrightness(uint8_t brightness) {
        _brightness = brightness;
      }

      uint8_t getBrightness() const {
        return _brightness;
      }

      void adjustBrightness(int8_t delta) {
        _brightness = static_cast<uint8_t>(constrain(static_cast<uint16_t>(_brightness) + delta, 0, 255));
      }

    private:
      uint32_t _stepMillis = DEFAULT_STEP_MILLIS;
      uint8_t  _brightness = DEFAULT_BRIGHTNESS;
  };

  template <typename pixels_t> class Sequence {
    public:
      Sequence(pixels_t* pixels, Configuration const* configuration) : _pixels(pixels), _configuration(configuration) {
        
      }

    void start() {
      _startMillis = millis();
      _nextStep = 0;
      startSelf();
      _pixels->show();
      _enabled = true;
    }

    void stop() {
      stopSelf();
      _pixels->show();
      _enabled = false;
    }

    void animate() {
      if (_enabled) {
        bool show = false;
        uint8_t brightness = _configuration->getBrightness();
        
        if (brightness != _pixels->getBrightness()) {
          _pixels->setBrightness(brightness);
          show = true;
        }
        
        uint32_t currentMillis = millis();
        uint32_t currentStep = (currentMillis - _startMillis) / _configuration->getStepMillis();

        if (currentStep >= _nextStep) {
          _nextStep = currentStep + 1;

          animateSelf(currentStep);
          show = true;
        }

        if (show) {
          _pixels->show();
        }
      }
    }

    protected:
      pixels_t& getPixels() {
        return *_pixels;
      }

    private:
      pixels_t*            _pixels;
      Configuration const* _configuration;
      uint32_t             _startMillis = 0;
      uint32_t             _nextStep = 0;
      bool                 _enabled = false;

      virtual void startSelf() {
        
      }

      virtual void stopSelf() {
        
      }

      virtual void animateSelf(uint32_t step) {
        
      }
  };

  template <typename pixels_t> class SolidColor final : public Sequence<pixels_t> {
    public:
      SolidColor(pixels_t* pixels, Configuration const* configuration, uint32_t color) : _color(color), Sequence<pixels_t>(pixels, configuration) {
        
      }

    private:
      uint32_t _color;
      
      void startSelf() override {
        this->getPixels().fill(_color);
      }
  };

  template <typename pixels_t> class ColorWipe final : public Sequence<pixels_t> {
    public:
      ColorWipe(pixels_t* pixels, Configuration const* configuration, uint32_t color) : _color(color), Sequence<pixels_t>(pixels, configuration) {
        
      }

    private:
      uint32_t _color;
      
      void animateSelf(uint32_t step) override {
        auto& pixels = this->getPixels();
        
        pixels.setPixelColor(step % pixels.numPixels(), _color);
      }
  };

  template <typename pixels_t> class RainbowCycle final : public Sequence<pixels_t> {
    public:
      RainbowCycle(pixels_t* pixels, Configuration const* configuration) : Sequence<pixels_t>(pixels, configuration) {
        
      }

    private:
      void animateSelf(uint32_t step) override {
        auto& pixels = this->getPixels();
        uint32_t firstPixelHue = step * 1000;
  
        for (int i = 0; i < pixels.numPixels(); ++i) {
          uint32_t pixelsHue = firstPixelHue + ((~i << 16L) / pixels.numPixels());
  
          pixels.setPixelColor(i, pixels.gamma32(pixels.ColorHSV(pixelsHue)));
        }
      }
  };

  template <typename pixels_t> class Pulse final : public Sequence<pixels_t> {
    public:
      Pulse(pixels_t* pixels, Configuration const* configuration, uint32_t color) : _color(color), Sequence<pixels_t>(pixels, configuration) {
        
      }

    private:
      uint32_t _color;

      void startSelf() {
        this->getPixels().fill(0);
      }

      void animateSelf(uint32_t step) override {
        int multiplier = step % 0x20;

        if ((step & 0x20) != 0) {
          multiplier = 0x20 - multiplier;
        }

        uint32_t color =
          (((((_color >> 24) & 0x0FF) * multiplier) / 0x20) << 24) |
          (((((_color >> 16) & 0x0FF) * multiplier) / 0x20) << 16) |
          (((((_color >>  8) & 0x0FF) * multiplier) / 0x20) <<  8) |
          (((((_color >>  0) & 0x0FF) * multiplier) / 0x20) <<  0);

        this->getPixels().fill(color);
      }
  };

  template <typename pixels_t> class Comet final : public Sequence<pixels_t> {
    public:
      Comet (pixels_t* pixels, Configuration const* configuration, uint32_t color) : Sequence<pixels_t>(pixels, configuration) {
        for (auto it = _colors.rbegin(); it != _colors.rend(); ++it) {
          *it = color;
          color = reduceBrightness(color);
        }
      }

    private:
      array<uint32_t, 4> _colors;

      uint32_t reduceBrightness(uint32_t color) {
        return (color >> 1) & 0x7F7F7F7F;
      }

      void startSelf() override {
        fillPixels(0);
      }

      void animateSelf(uint32_t step) override {
        fillPixels(step);
      }

      void fillPixels(uint32_t step) {
        auto numPixels = this->getPixels().numPixels();

        for (uint16_t i = 0; i < numPixels; ++i) {
          auto pixel = static_cast<uint16_t>((step + i) % numPixels);

          if (i < _colors.size()) {
            this->getPixels().setPixelColor(pixel, _colors[i]);
          } else {
            this->getPixels().setPixelColor(pixel, 0);
          }
        }
      }
  };

  template <template <typename pixels_t> typename sequence_t, typename pixels_t, typename... args_t>
  sequence_t<pixels_t> make_sequence(pixels_t* pixels, Configuration* configuration, args_t&&... args) {
    return sequence_t<pixels_t>(pixels, configuration, forward<args_t>(args)...);
  }
}
