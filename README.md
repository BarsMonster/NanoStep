# NanoStep
Closed loop stepper motor driver firmware for the BTT S42B-V1.0/V1.1 board. Should be relatively easy to tweak for v2.0 board, but I miss hardware.

My original goal was increasing angular precision of servomotor, targeting high-precision Z-axis of 3D printers. 

While looking through all forks of S42B I realized that they share same fundamental flaw : PID is controlling effort (current), and much less care is given to positional accuracy. What happened was while error was small - current was dropping to 0. And when steps start comming in - they got missed due to too low current. 

Tested with no missed steps at 50kHz step signal.

## Key changes
- Calibration is no longer needed. Autocalibration is enabled in TLE5012. Requires 1.5 rotation for calibration. For perfect results - you can do Z axis move in the beginning of the print. Instead of calibration there is now "Reset" which resets settings to default. 
- TLE5012 now utilizes full 15-bit resolution vs 14-bit before. FIR filter further improves results. 
- EN signal is processed in interrupt. Dramatically lower latency. 
- DIR signal latency is reduced - less code in interrupt processing. Still around 2µs latency is expected (note that hardware steppers has around 0.2µs latency). But should be good enough. 
- No more floating point math in control loop. Perfect fixed point math everywhere. 
- Main control loop is optimized and now runs at 30kHz vs 10kHz originally. 35-40kHz also fits, but I prefer to have some margin. 
- OLED display is >10x faster. 
- DIP pins are ignored. They are a mess. Please use OLED menu or serial port for configuration. 
- Magnetic Viewer added which shows how strong is magnetic field. You should be between 40% and 80% field strength maximum. 
- No limit on error magnitude to be corrected. You can force it 5 rotations away from target - and it will return back (original version gave up after ~40° error).
- Auto-restart if multiple skipped steps detected. 
- Only PI values are used. D is currently ignored. 
- Current is fixed, will no longer reduce automatically. If you want to run cooler - feel free to reduce current below manufacturer recomendation. 50% current works just fine. 
- Display always shows encoder reading - useful for debugging. 
- Screensaver will switch off display after 1 minute of inactivity (en=0 or no steps)

## Features from TrueStep
- New UART [interface](SerialInterface.md) 
  - Reduced packet overheads
  - Sequence counter to detect missing packets
  - CRC checksum
- New OLED Menu
  - Simpler navigation
  - Adjustment of PID gains

To ease configuration and to access more advanced features you can use [TrueStepTerminal](utils/TrueStepTerminal).
