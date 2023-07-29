# Smart-Home

- **Microprocessors course project (Spring 2023)**
- In this project, I used 2 ATMega32 microcontrollers and SPI protocol between master and slave to transmit data as serial.
- I used  Proteus to simulat project.
- This system has three main objectives:

<p align="center">
		<img src="https://github.com/matinmonshizadeh/Smart-Home/assets/96329489/d755ccc7-6143-4bc2-bf74-943d6899b353" />
</p>


## 1. Security
A default password is hard coded and users can enter a password using a keypad. Every time they
press a button a star symbol is displayed on the LCD. When they entered the desired password, they
can submit the password by pressing * on the keypad. They can also edit the entered password by
pressing # which deletes the last character on each press.
But what if the users want to check if they entered the correct password before submitting? This
is where a push button comes into the action. The button acts as a toggle switch for showing the
password. If the entered password is invisible and a user presses the button the LCD should be
cleared, and the entered password must be printed on it. When the password is visible and the user
presses the button, LCD should be cleared, and a star symbol should be printed for each entered
number. If show password is active, newly entered characters should also be displayed.
After submitting the password two scenarios happen: the password is either correct or wrong.
1. If the password is correct, “Access is granted” should be displayed on the LCD. If and only if
the password is correct, access to the other parts such as temperature and lighting control is
granted.
2. If the password is wrong, “Wrong password” should be displayed. The message should disappear
after one second and the system should receive a new password again. Other parts of the system
MUST NOT work unless the correct password is entered.






## 2. Temperature Control
The goal of this part is to design a temperature monitoring system that controls and regulates the
temperature of the house.
We have two motors: the cooler and the heater. For temperatures between 25 and 55 degrees,
the cooler motor must be turned on, starting with a duty cycle of 50% plus 10% for every additional
5 degrees. (E.g., for a temperature of 37 degrees, the duty cycle is 70%). For temperatures between
0 and 20, the heater motor must be turned on, starting with a duty cycle of 100% minus 25% for
every additional 5 degrees. Between 20 and 25 degrees, neither the cooler nor the heater is turned
on.

We also have a red warning LED and a blue warning LED. The red LED must blink if the
temperature is higher than 55 degrees (the cooler must stop working at this point). The blue LED
must blink if the temperature is lower than 0 degrees (the heater must stop working at this point).




## 3. Lightning Control
The goal of this part is to design a lightning monitoring system that controls and regulates the
lightning in the house.

The lighting control subsystem has one motor. Light is denoted by a number between 0 and 100.
For light intensity between 0 and 25, the motor works with a duty cycle of 100%; for every 25%
increase in light, the motor’s duty cycle is reduced by a factor of 25%.
