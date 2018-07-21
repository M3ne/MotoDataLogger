# MotoDataLogger
The purpose of the project is develop a datalogger for motorbike and in particular for the data logging using a specific hardware of data about the position in the space, gear, traction and temperature of the power train. Data retrived will be analyzed with a specific application or exported to be visualized by popular tools as i2 or windarab.

<b>Hardware implementation:</b>
<br>
<ul>
<li>Teensy 3.2 board <br>
<img src="https://c.76.my/Malaysia/teensy-3-2-development-board-kagstore-1705-08-kagstore@6.jpg">
</li>
  <li>SparkFun Thermocouple Breakout - MAX31855K <br> <img src="https://cdn.sparkfun.com//assets/parts/1/0/4/8/7/13266-01.jpg"></li>
  <li>SparkFun GPS Logger Shield<br><img src="https://cdn.sparkfun.com//assets/parts/1/1/2/8/4/13750-01.jpg"></li>
  <li>SparkFun 6 Degrees of Freedom Breakout - LSM6DS3<br><img src="https://cdn.sparkfun.com//assets/parts/1/0/6/4/6/13339-01.jpg"></li>
  <li>SparkFun Battery Babysitter - LiPo Battery Manager<br><img src="https://cdn.sparkfun.com//assets/parts/1/1/3/3/1/13777-01.jpg"></li>
  <li>SparkFun Opto-isolator Breakout<br><img src="https://cdn.sparkfun.com//assets/parts/2/5/2/2/09118-02.jpg"></li>
  <li>NOR Flash 128M 2.3-3.6V 166MHz Serial Flash<br><img src="https://www.mouser.it/images/mouserelectronics/images/SOIC_8_t.jpg"></li>
  </ul>
  
  <b> Software development</b>
  <ul>
  <li>Arduino IDE <br><img src="http://3.bp.blogspot.com/-etcRcSlYRR0/VUuVUl--rgI/AAAAAAAANXw/uyqI9tcZ050/s1600/Arduino%2B1.6.4.png">
  <br> Using libraries developed for the different modules, the integration and the store of the data is implemented using Arduino IDE. The communication to external application is made through USB</li>
  <li>Qt framework <br><img src="http://tuxnews.it/wp-content/uploads/2015/11/Qt-logo.png">
  <br> Using Qt framework, the will is develop a software tool able to retrieve information from the dataloger and finaly export for easy analysis using i2 or WinDarab software</li>
