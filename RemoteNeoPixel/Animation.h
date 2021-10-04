#include <array>
#include <utility>

#define BLACK_COLOR 0x000000
#define RED_COLOR   0xFF0000
#define GREEN_COLOR 0x00FF00
#define BLUE_COLOR  0x0000FF

namespace anime {
  using namespace std;

  namespace detail {
    constexpr uint8_t getWhite(uint32_t color) {
      return static_cast<uint8_t>((color >> 24) & 0xFF);
    }

    constexpr uint8_t getRed(uint32_t color) {
      return static_cast<uint8_t>((color >> 16) & 0xFF);
    }

    constexpr uint8_t getGreen(uint32_t color) {
      return static_cast<uint8_t>((color >> 8) & 0xFF);
    }

    constexpr uint8_t getBlue(uint32_t color) {
      return static_cast<uint8_t>(color & 0xFF);
    }

    constexpr uint32_t colorRGBW(int32_t red, int32_t green, int32_t blue, int32_t white) {
      return ((static_cast<uint32_t>(white) & 0xFF) << 24) |
             ((static_cast<uint32_t>(red)   & 0xFF) << 16) |
             ((static_cast<uint32_t>(green) & 0xFF) <<  8) |
             ((static_cast<uint32_t>(blue)  & 0xFF));
    }
  }
  
  class Configuration {
    public:
      static const uint32_t DEFAULT_STEP_MILLIS = 50ul;
      static const uint8_t  DEFAULT_BRIGHTNESS  = 32;

      Configuration() = default;

      Configuration(initializer_list<uint32_t> colors) {
        auto iter = begin(colors);
        for (size_t index = 0; index < colors.size(); ++index) {
          if (index >= _colors.size()) {
            break;
          }

          _colors[index] = *iter++;
        }
      }

      void setColor(size_t index, uint32_t color) {
        if (index < _colors.size()) {
          _colors[index] = color;
        }
      }

      uint32_t getColor(size_t index) const {
        if (index < _colors.size()) {
          return _colors[index];
        }
        return BLACK_COLOR;
      }

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
      array<volatile uint32_t, 2> _colors = { BLACK_COLOR, BLACK_COLOR };
      volatile uint32_t _stepMillis = DEFAULT_STEP_MILLIS;
      volatile uint8_t  _brightness = DEFAULT_BRIGHTNESS;
  };

  template <typename pixels_t> class Sequence {
    public:
      Sequence(pixels_t* pixels, Configuration const* configuration) : _pixels(pixels), _configuration(configuration), _lastColor(configuration->getColor(0)) {
        
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
        uint32_t nextColor = _configuration->getColor(0);

        if (nextColor != _lastColor) {
          changeColorSelf(nextColor);
          _lastColor = nextColor;
        }
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

      Configuration const& getConfiguration() const {
        return *_configuration;
      }

    private:
      pixels_t*            _pixels;
      Configuration const* _configuration;
      uint32_t             _lastColor;
      uint32_t             _startMillis = 0;
      uint32_t             _nextStep = 0;
      bool                 _enabled = false;

      virtual void startSelf() {
        
      }

      virtual void stopSelf() {
        
      }

      virtual void animateSelf(uint32_t step) {
        
      }

      virtual void changeColorSelf(uint32_t newColor) {

      }
  };

  template <typename pixels_t> class SolidColor final : public Sequence<pixels_t> {
    public:
      SolidColor(pixels_t* pixels, Configuration const* configuration) : Sequence<pixels_t>(pixels, configuration) {
        
      }

    private:
      void startSelf() override {
        this->getPixels().fill(this->getConfiguration().getColor(0));
      }

      void changeColorSelf(uint32_t newColor) override {
        this->getPixels().fill(newColor);
      }
  };

  template <typename pixels_t> class ColorWipe final : public Sequence<pixels_t> {
    public:
      ColorWipe(pixels_t* pixels, Configuration const* configuration) : Sequence<pixels_t>(pixels, configuration) {
        
      }

    private:
      void startSelf() override {
        this->getPixels().fill(BLACK_COLOR);
      } 

      void animateSelf(uint32_t step) override {
        auto& pixels = this->getPixels();
        
        pixels.setPixelColor(step % pixels.numPixels(), this->getConfiguration().getColor(0));
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
      Pulse(pixels_t* pixels, Configuration const* configuration) : Sequence<pixels_t>(pixels, configuration) {
        
      }

    private:
      void startSelf() override {
        this->getPixels().fill(BLACK_COLOR);
      }

      void animateSelf(uint32_t step) override {
        int multiplier = step % 0x20;

        if ((step & 0x20) != 0) {
          multiplier = 0x20 - multiplier;
        }

        uint32_t color = this->getConfiguration().getColor(0);
        color = detail::colorRGBW(
          ((detail::getRed(color)   * multiplier) / 0x20),
          ((detail::getGreen(color) * multiplier) / 0x20),
          ((detail::getBlue(color)  * multiplier) / 0x20),
          ((detail::getWhite(color) * multiplier) / 0x20));

        this->getPixels().fill(color);
      }
  };

  template <typename pixels_t> class Fade final : public Sequence<pixels_t> {
    public:
      Fade(pixels_t* pixels, Configuration const* configuration) : Sequence<pixels_t>(pixels, configuration) {
        initialize();
      }

    private:
      int16_t _redDiff;
      int16_t _greenDiff;
      int16_t _blueDiff;
      int16_t _whiteDiff;

      void initialize() {
        auto& config = this->getConfiguration();
        uint32_t color0 = config.getColor(0);
        uint32_t color1 = config.getColor(1);

        _redDiff   = detail::getRed(color0)   - detail::getRed(color1);
        _greenDiff = detail::getGreen(color0) - detail::getGreen(color1);
        _blueDiff  = detail::getBlue(color0)  - detail::getBlue(color1);
        _whiteDiff = detail::getWhite(color0) - detail::getWhite(color1);
      }
      
      void startSelf() override {
        this->getPixels().fill(this->getConfiguration().getColor(0));
      }

      void animateSelf(uint32_t step) override {
        auto& pixels = this->getPixels();
        int multiplier = pixels.sine8(static_cast<uint8_t>((step << 2) + 0x40));

        auto& config = this->getConfiguration();
        uint32_t color0 = config.getColor(0);

        uint32_t color = detail::colorRGBW(
          ((_redDiff * multiplier)   / 255) + detail::getRed(color0),
          ((_greenDiff * multiplier) / 255) + detail::getGreen(color0),
          ((_blueDiff * multiplier)  / 255) + detail::getBlue(color0),
          ((_whiteDiff * multiplier) / 255) + detail::getWhite(color0));

        this->getPixels().fill(color);
      }
  };

  template <typename pixels_t> class Comet final : public Sequence<pixels_t> {
    public:
      Comet(pixels_t* pixels, Configuration const* configuration) : Sequence<pixels_t>(pixels, configuration) {
        fillColors(this->getConfiguration().getColor(0));
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

      void changeColorSelf(uint32_t newColor) {
        fillColors(newColor);
      }

      void fillColors(uint32_t color) {
        for (auto it = _colors.rbegin(); it != _colors.rend(); ++it) {
          *it = color;
          color = reduceBrightness(color);
        }
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
