#include "clockApp.h"
uint8_t alarmHours[5];
uint8_t alarmMins[5];
uint8_t alarmEnabled[5] = {2, 2, 2, 2, 2};
bool alarmRepeat[5] = {0,0,0,0,0};
bool alarmRepeatDays[5][7] = {
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0}
};
String alarmTrack[5] = {
    "alarm.wav", 
    "alarm.wav", 
    "alarm.wav", 
    "alarm.wav", 
    "alarm.wav"
};
void clockApp()
{
	String clockItems[4] = {
		"Alarm",
		"Clock",
		"Stopwatch",
		"Timer"
	};
	while(1)
	{
		int8_t index = clockMenu(clockItems, 4);
		if(index == -1)
			break;
		switch(index)
		{
			case 0:
				clockAlarm();
			break;

			case 1:
			{
				uint32_t timer = millis();
				bool blinkState = 0;
				String temp = "";
				String monthsList[] PROGMEM = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
				while(!mp.buttons.released(BTN_B) && !mp.buttons.released(BTN_A))
				{
					mp.display.fillScreen(0x963F);
					// date and time
					mp.updateTimeRTC();
					mp.display.setTextFont(2);
					mp.display.setTextSize(2);
					mp.display.setTextColor(TFT_BLACK);
					mp.display.setCursor(15, 25);
					temp = "";
					if (mp.clockHour < 10)
						temp.concat("0");
					temp.concat(mp.clockHour);
					temp.concat(":");
					if (mp.clockMinute < 10)
						temp.concat("0");
					temp.concat(mp.clockMinute);
					temp.concat(":");
					if (mp.clockSecond < 10)
						temp.concat("0");
					temp.concat(mp.clockSecond);
					
					mp.display.printCenter(temp);
					mp.display.setTextSize(1);
					mp.display.setCursor(63, 85);
					temp = "";
					if (mp.clockDay < 10)
						temp.concat("0");
					temp.concat(mp.clockDay);
					if(mp.clockDay < 20 && mp.clockDay > 10)
						temp.concat("th");
					else if(mp.clockDay%10 == 1)
						temp.concat("st");
					else if(mp.clockDay%10 == 2)
						temp.concat("nd");
					else if(mp.clockDay%10 == 3)
						temp.concat("rd");
					else
						temp.concat("th");
					temp.concat(" of ");
					temp.concat(monthsList[mp.clockMonth - 1]);

					mp.display.printCenter(temp);
					mp.display.setCursor(0,100);
					mp.display.printCenter(2000 + mp.clockYear);


					if(millis()-timer >= 1000)
					{
						blinkState = !blinkState;
						timer = millis();
					}
					mp.update();
					
				}
				while(!mp.update());
			}
			break;

			case 2:
				clockStopwatch();
			break;

			case 3:
				clockTimer();
			break;

		}
	}
}
int8_t clockMenu(String* title, uint8_t length) {
	uint8_t offset = 4;
	bool pressed = 0;
	uint8_t cursor = 0;
	int32_t cameraY = 0;
	int32_t cameraY_actual = 0;
	mp.dataRefreshFlag = 0;
	uint8_t boxHeight = 30; //actually 2 less than that
	while (1) {
		while (!mp.update());
		mp.display.fillScreen(TFT_BLACK);
		mp.display.setCursor(0, 0);
		cameraY_actual = (cameraY_actual + cameraY) / 2;
		if (cameraY_actual - cameraY == 1) {
			cameraY_actual = cameraY;
		}

		for (uint8_t i = 0; i < length; i++) {
			clockMenuDrawBox(title[i], i, cameraY_actual);
		}
		uint8_t y = cameraY_actual;
		uint8_t i = cursor;
		if (millis() % 500 <= 250 && pressed == 0);
		else
		{
			y += i * boxHeight + offset;
			mp.display.drawRect(0, y-1, mp.display.width()-1, boxHeight+2, TFT_RED);
			mp.display.drawRect(1, y, mp.display.width()-3, boxHeight, TFT_RED);
		}
		
		

		if (mp.buttons.kpd.pin_read(BTN_DOWN) == 1 && mp.buttons.kpd.pin_read(BTN_UP) == 1)
			pressed = 0;

		if (mp.buttons.released(BTN_A)) {   //BUTTON CONFIRM
			osc->note(75, 0.05);
			osc->play();
			while (!mp.update());// Exit when pressed
			break;
		}

		if (mp.buttons.released(BTN_UP)) {  //BUTTON UP
			osc->note(75, 0.05);
			osc->play();
			if (cursor == 0) {
				cursor = length - 1;
				if (length > 6) {
					cameraY = -(cursor - 2) * boxHeight;
				}
			}
			else {
				cursor--;
				if (cursor > 0 && (cursor * boxHeight + cameraY + offset) < boxHeight) {
					cameraY += 15;
				}
			}
			pressed = 1;
		}

		if (mp.buttons.released(BTN_DOWN)) { //BUTTON DOWN
			osc->note(75, 0.05);
			osc->play();
			cursor++;
			if ((cursor * boxHeight + cameraY + offset) > 128) {
				cameraY -= boxHeight;
			}
			if (cursor >= length) {
				cursor = 0;
				cameraY = 0;

			}
			pressed = 1;
		}


		if (mp.buttons.released(BTN_B) == 1) //BUTTON BACK
		{
			return -1;
		}
	}

	return cursor;

}
void clockMenuDrawBox(String title, uint8_t i, int32_t y) {
	uint8_t offset = 4;
	uint8_t boxHeight;
	boxHeight = 30;
	y += i * boxHeight + offset;
	if (y < 0 || y > mp.display.width()) {
		return;
	}
	mp.display.fillRect(2, y + 1, mp.display.width() - 4, boxHeight-2,TFT_DARKGREY);
	if (title == "Alarm")
	{
		mp.display.drawBitmap(5, y + 2, alarmIcon, 0xFC92, 2);
		mp.display.setTextColor(0xFC92);
		// mp.display.fillRect(2, y + 1, mp.display.width() - 4, boxHeight-2, 0xFC92);

	}
	else if (title == "Clock") 
	{
		mp.display.drawBitmap(5, y + 2, clockIcon, 0x963F, 2);
		mp.display.setTextColor(0x963F);
		// mp.display.fillRect(2, y + 1, mp.display.width() - 4, boxHeight-2, 0x963F);
	}
	else if (title == "Stopwatch")
	{
		mp.display.drawBitmap(5, y + 2, stopwatchIcon, 0xFF92, 2);
		mp.display.setTextColor(0xFF92);
		// mp.display.fillRect(2, y + 1, mp.display.width() - 4, boxHeight-2, 0xFF92);

	}
	else if (title == "Timer")
	{
		mp.display.drawBitmap(5, y + 2, timerIcon, 0x97F6, 2);
		mp.display.setTextColor(0x97F6);
		// mp.display.fillRect(2, y + 1, mp.display.width() - 4, boxHeight-2, 0x97F6);

	}
	// mp.display.setTextColor(TFT_BLACK);
	mp.display.setTextSize(2);
	mp.display.setTextFont(1);
	mp.display.drawString(title, 40, y +  8);
	mp.display.setTextColor(TFT_WHITE);
	mp.display.setTextSize(1);
}
void clockStopwatch()
{
	bool running = 0;
	String temp;
	uint32_t timeMillis = 0;
	uint32_t timeActual = 0;
	char key;
	uint32_t blinkMills = millis();
	bool blinkState = 1;
	while(!mp.buttons.released(BTN_B))
	{
		key = mp.buttons.kpdNum.getKey();
		if(key != NO_KEY)
		{
			Serial.println(key);
			delay(5);
		}
		mp.display.setTextColor(TFT_BLACK);
		mp.display.fillScreen(0xFF92);
		mp.display.setTextFont(2);
		mp.display.setTextSize(1);
		mp.display.setCursor(123,110);
		mp.display.print("Reset");
		mp.display.setTextFont(2);
		mp.display.setTextSize(2);
		mp.display.setCursor(15, 25);
		int seconds = timeActual / 1000;
		int centiseconds = timeActual % 1000 / 10;
		temp = "";
		if(seconds > 59)
		{
			int mins = seconds / 60;
			if (mins < 10)
				temp.concat("0");
			temp.concat(mins);
			temp.concat(":");
		}
		if (seconds % 60 < 10)
			temp.concat("0");
		temp.concat(seconds % 60);
		temp.concat(":");
		if (centiseconds < 10)
			temp.concat("0");
		temp.concat(centiseconds);
		mp.display.printCenter(temp);

		if(!blinkState)
		{
			if(seconds > 59)
			{
				mp.display.fillRect(0, 0, 56, 60, 0xFF92);
				mp.display.fillRect(64, 0, 33, 60, 0xFF92);
				mp.display.fillRect(102, 0, 50, 60, 0xFF92);
			}
			else
			{
				mp.display.fillRect(0, 0, 75, 60, 0xFF92);
				mp.display.fillRect(82, 0, 70, 60, 0xFF92);
			}
		}
		// if(blinkState)
		// else
		// 	mp.display.printCenter(":");
		if(!running)
		{
			mp.display.drawBitmap(72, 90, pause2, TFT_BLACK, 2);
			if(mp.buttons.released(BTN_A))
			{
				blinkState = 1;
				blinkMills = millis();
				running = 1;
				timeMillis = millis() - timeActual;
			}
			if(key == 'A')
			{
				timeMillis = 0;
				timeActual = 0;
			}
			while (!mp.update());
			if (millis() - blinkMills >= 350)
			{
				blinkMills = millis();
				blinkState = !blinkState;
			}
		}
		if(running)
		{
			mp.display.drawBitmap(72, 88, play, TFT_BLACK, 2);

			timeActual = millis() - timeMillis;
			if(mp.buttons.released(BTN_A))
			{
				running = 0;
				timeMillis = millis();
			}
			if(key == 'A')
			{
				running = 0;
				timeMillis = 0;
				timeActual = 0;
			}
			while (!mp.update());
		}
		mp.display.printCenter(temp);
		mp.update();
	}
	while(!mp.update());
}

void clockAlarm()
{
	loadAlarms();
	uint16_t alarmCount = 0;
	for (int i = 0; i < 5;i++)
	{
		if(alarmEnabled[i] != 2)
			alarmCount++;
	}
	uint8_t alarmsArray[alarmCount];
	uint8_t temp = 0;
	for (int i = 0; i < 5;i++)
	{
		if(alarmEnabled[i] != 2)
		{
			alarmsArray[temp] = i;
			temp++;
		}
	}
	while(1)
	{
		int8_t index = clockAlarmMenu(alarmsArray, alarmCount + 1) - 1;
		if(index == -1)
		{
			int8_t newAlarm = -1;
			for(int i = 0;i<5;i++)
			{
				if(alarmEnabled[i] == 2)
				{
					newAlarm = i;
					break;
				}
			}
			if(newAlarm == -1)
			{
				mp.display.setTextColor(TFT_BLACK);
				mp.display.setTextSize(1);
				mp.display.setTextFont(2);
				mp.display.drawRect(14, 45, 134, 38, TFT_BLACK);
				mp.display.drawRect(13, 44, 136, 40, TFT_BLACK);
				mp.display.fillRect(15, 46, 132, 36, 0xFC92);
				mp.display.setCursor(47, 55);
				mp.display.printCenter("Limit reached!");
				uint32_t tempMillis = millis();
				while(millis() < tempMillis + 3000)
				{
					mp.update();
					if(mp.buttons.pressed(BTN_A) || mp.buttons.pressed(BTN_B))
					{
						while(!mp.buttons.released(BTN_A) && !mp.buttons.released(BTN_B))
							mp.update();
						break;
					}
				}
			}
			else
			{
				clockAlarmEdit(newAlarm);
				alarmCount = 0;
				for (int i = 0; i < 5;i++)
				{
					if(alarmEnabled[i] != 2)
						alarmCount++;
				}
				alarmsArray[alarmCount];
				temp = 0;
				for (int i = 0; i < 5;i++)
				{
					if(alarmEnabled[i] != 2)
					{
						alarmsArray[temp] = i;
						temp++;
					}
				}
			}
			
		}
		else if(index == -2)
			break;
		else if(index < -2)
		{
			index = alarmsArray[-(index + 4)];
			Serial.printf("Deleting alarm on index %d\n", index);
			alarmEnabled[index] = 2;
			alarmHours[index] = 12;
			alarmMins[index] = 0;
			alarmRepeat[index] = 0;
			for (int i = 0;i<7;i++)
				alarmRepeatDays[index][i] = 0;
			alarmTrack[index] = "/alarm.wav";
			alarmCount = 0;
			for (int i = 0; i < 5;i++)
			{
				if(alarmEnabled[i] != 2)
					alarmCount++;
			}
			temp = 0;
			for (int i = 0; i < 5;i++)
			{
				if(alarmEnabled[i] != 2)
				{
					alarmsArray[temp] = i;
					temp++;
				}
			}
			saveAlarms();
		}
		else
		{
			clockAlarmEdit(index);
			alarmCount = 0;
			for (int i = 0; i < 5;i++)
			{
				if(alarmEnabled[i] != 2)
					alarmCount++;
			}
			temp = 0;
			for (int i = 0; i < 5;i++)
			{
				if(alarmEnabled[i] != 2)
				{
					alarmsArray[temp] = i;
					temp++;
				}
			}
		}
	}
}
int8_t clockAlarmMenu(uint8_t* alarmsArray, uint8_t length) {
	uint8_t offset = 1;
	uint8_t cursor = 0;
	int32_t cameraY = 0;
	int32_t cameraY_actual = 0;
	uint8_t bottomBezel = 30;
	mp.dataRefreshFlag = 0;
	char key;
	uint8_t boxHeight;
	boxHeight = 28; //actually 2 less than that
	while (1) {
		while (!mp.update());
		key = mp.buttons.kpdNum.getKey();
		mp.display.fillScreen(0xFC92);
		mp.display.setCursor(0, 0);
		cameraY_actual = (cameraY_actual + cameraY) / 2;
		if (cameraY_actual - cameraY == 1) {
			cameraY_actual = cameraY;
		}

		for (uint8_t i = 0; i < length; i++) {
			if(i < 1)
			{
				uint8_t temp = cameraY_actual;
				temp += i * boxHeight + offset;
				mp.display.fillRect(2, temp + 1, mp.display.width() - 4, boxHeight-2,TFT_DARKGREY);
				mp.display.setTextColor(TFT_WHITE);
				mp.display.setTextFont(2);
				mp.display.setTextSize(3);
				mp.display.setCursor(0, temp-12);
				mp.display.printCenter("+");
				
			}
			else
			{
				if(cameraY_actual < 128-bottomBezel)
					clockAlarmMenuDrawBox(alarmsArray[i-1], i, cameraY_actual);
			}
		}
		uint8_t y = cameraY_actual;
		uint8_t i = cursor;
		if (millis() % 500 <= 250);
		else
		{
			y += i * boxHeight + offset;
			mp.display.drawRect(0, y-1, mp.display.width()-1, boxHeight+2, TFT_RED);
			mp.display.drawRect(1, y, mp.display.width()-3, boxHeight, TFT_RED);
		}
		mp.display.fillRect(0,114, 160, 22, 0xFC92);
		mp.display.setCursor(2, 113);
		mp.display.setTextFont(2);
		mp.display.setTextSize(1);
		mp.display.setTextColor(TFT_BLACK);
		mp.display.print("Delete");
		if (mp.buttons.released(BTN_A)) {   //BUTTON CONFIRM
			osc->note(75, 0.05);
			osc->play();
			while (!mp.update());// Exit when pressed
			break;
		}

		if (mp.buttons.released(BTN_UP)) {  //BUTTON UP
			osc->note(75, 0.05);
			osc->play();
			if (cursor == 0) {
				cursor = length - 1;
				if (length > 4) {
					cameraY = -(cursor - 3) * boxHeight;
				}
			}
			else {
				cursor--;
				if (cursor > 0 && (cursor * boxHeight + cameraY + offset) < boxHeight) {
					cameraY += boxHeight;
				}
			}
		}

		if (mp.buttons.released(BTN_DOWN)) { //BUTTON DOWN
			osc->note(75, 0.05);
			osc->play();
			cursor++;
			if ((cursor * boxHeight + cameraY + offset) > 128 - bottomBezel) {
				cameraY -= boxHeight;
			}
			if (cursor >= length) {
				cursor = 0;
				cameraY = 0;

			}
		}
		
		if (mp.buttons.released(BTN_B)) //BUTTON BACK
			return -1;
		if(key == 'C' && cursor > 0)
		{
			return -(cursor + 2);
		}
	}
	return cursor;
}
void clockAlarmMenuDrawBox(uint8_t alarmIndex, uint8_t i, int32_t y) {
	uint8_t offset = 1;
	uint8_t boxHeight;
	boxHeight = 28;
	y += i * boxHeight + offset;
	if (y < 0 || y > mp.display.width()) {
		return;
	}
	mp.display.fillRect(2, y + 1, mp.display.width() - 4, boxHeight-2,TFT_DARKGREY);
	mp.display.setTextFont(2);
	mp.display.setTextSize(2);
	mp.display.setCursor(5, y-2);
	if (alarmHours[alarmIndex] < 10)
		mp.display.print("0");
	mp.display.print(alarmHours[alarmIndex]);
	mp.display.print(":");
	if (alarmMins[alarmIndex] < 10)
		mp.display.print("0");
	mp.display.print(alarmMins[alarmIndex]);
	mp.display.setTextSize(1);
	mp.display.setCursor(130, y + 7);
	mp.display.print(alarmEnabled[alarmIndex] ? "ON" : "OFF");

	mp.display.setCursor(80, y + 11);
	mp.display.print(alarmRepeat[alarmIndex] ? "repeat" : "once");
}
void clockAlarmEdit(uint8_t index)
{
	bool enabled = 0;
	bool repeat = 0;
	uint8_t hours = 12;
	uint8_t mins = 0;
	String temp;
	bool days[7] = {0, 0, 0, 0, 1, 1, 0};
	uint8_t cursorX = 0;
	uint8_t cursorY = 0;
	char key;
	uint32_t blinkMillis = millis();
	uint32_t color = TFT_BLACK;
	bool blinkState = 1;
	String parsedAlarmTrack = "alarm.wav";
	String localAlarmTrack = "/Music/alarm.wav";

	if(alarmEnabled[index] != 2)
	{
		hours = alarmHours[index];
		mins = alarmMins[index];
		enabled = alarmEnabled[index];
		localAlarmTrack = alarmTrack[index];
		for(int i = 0; i < 7; i++)
		{
			days[i] = alarmRepeatDays[index][i];
		}
		repeat = alarmRepeat[index];
	}
	while(1)
	{
		color = TFT_BLACK;
		key = mp.buttons.kpdNum.getKey();
		mp.display.fillScreen(0xFC92);
		//Hour black
		mp.display.setTextColor(TFT_BLACK);
		mp.display.setCursor(15, 8);
		mp.display.setTextFont(2);
		mp.display.setTextSize(2);
		temp = "";
		if (hours < 10)
			temp.concat("0");
		temp.concat(hours);
		temp.concat(":");
		if (mins < 10)
			temp.concat("0");
		temp.concat(mins);
		mp.display.print(temp);
		mp.display.drawRect(115, 15, 20, 20, TFT_BLACK);
		mp.display.drawRect(116, 16, 18, 18, TFT_BLACK);
		if(enabled)
		{
			mp.display.setTextFont(1);
			mp.display.setTextSize(2);
			mp.display.setCursor(120, 18);
			mp.display.print("X");
			mp.display.setTextFont(2);
		}
		else
			color = TFT_DARKGREY;
		mp.display.setTextColor(color);
		mp.display.setCursor(15,45);
		mp.display.setTextSize(1);
		if(enabled)
		{
			if(!repeat)
			{
				mp.display.setCursor(42, 45);
				mp.display.print("once/");
				mp.display.setTextColor(TFT_DARKGREY);
				mp.display.print("repeat");
				mp.display.setCursor(85,63);
				mp.display.printCenter("M T W T F S S");
				temp = "";
				for(int i = 0; i<7;i++)
				{
					temp.concat(days[i] ? "X" : "O");
					if(i < 6)
						temp.concat(" ");
				}
				mp.display.setCursor(0,78);
				mp.display.printCenter(temp);
				mp.display.setTextColor(TFT_BLACK);
			}
			else
			{
				mp.display.setCursor(42, 45);
				mp.display.setTextColor(TFT_DARKGREY);
				mp.display.print("once");
				mp.display.setTextColor(TFT_BLACK);
				mp.display.print("/repeat");
				mp.display.setCursor(85,63);
				mp.display.printCenter("M T W T F S S");
				temp = "";
				for(int i = 0; i<7;i++)
				{
					temp.concat(days[i] ? "X" : "O");
					if(i < 6)
						temp.concat(" ");
				}
				mp.display.setCursor(0,78);
				mp.display.printCenter(temp);
			}

		}
		else
		{
			mp.display.printCenter("once/repeat");
			mp.display.setCursor(85,63);
			mp.display.printCenter("M T W T F S S");
			temp = "";
			for(int i = 0; i<7;i++)
			{
				temp.concat(days[i] ? "X" : "O");
				if(i < 6)
					temp.concat(" ");
			}
			mp.display.setCursor(0,78);
			mp.display.printCenter(temp);
		}
		mp.display.drawRect(20, 98, 120, 20, color);
		mp.display.drawRect(19, 97, 122, 22, color);
		mp.display.setCursor(0,100);
		mp.display.printCenter(parsedAlarmTrack);
		if(millis()-blinkMillis >= 350)
		{
			blinkState = !blinkState;
			blinkMillis = millis();
		}
		switch (cursorY)
		{
			case 0:
				if(key != NO_KEY)
				{
					blinkState = 1;
					blinkMillis = millis();
				}
				switch (cursorX)
				{
					case 0:
						if(key == 'C')
							hours /= 10;
						else if (key != 'B' && key != 'D' && key != '#' && key != '*' && key != NO_KEY && hours < 10)
							hours = hours * 10 + key - 48;
						if(!blinkState)
							mp.display.fillRect(0, 0, 46, 50, 0xFC92);
					break;
					case 1:
						if(key == 'C')
							mins /= 10;
						else if (key != 'B' && key != 'D' && key != '#' && key != '*' && key != NO_KEY && mins < 10)
							mins = mins * 10 + key - 48;
						if(!blinkState)
							mp.display.fillRect(51, 0, 40, 45, 0xFC92);
					break;
					case 2:
						if(!blinkState)
						{
							mp.display.drawRect(115, 15, 20, 20, 0xFC92);
							mp.display.drawRect(116, 16, 18, 18, 0xFC92);
						}
						if(mp.buttons.released(BTN_A))
						{
							while(!mp.update());
							enabled = !enabled;
							blinkState = 1;
							blinkMillis = millis();
						}
					break;
				}
				
				if(mp.buttons.released(BTN_RIGHT) && cursorX < 2)
				{
					blinkState = 0;
					blinkMillis = millis();
					cursorX++;
					mins %= 60;
					hours %= 24;
					while(!mp.update());
				}
				if(mp.buttons.released(BTN_LEFT) && cursorX > 0)
				{
					blinkState = 0;
					blinkMillis = millis();
					cursorX--;
					mins %= 60;
					hours %= 24;
					while(!mp.update());
				}
			break;

			case 1:
				cursorX = repeat;
				if(!blinkState)
				{
					if(cursorX == 0)
						mp.display.fillRect(0, 50, 71, 12, 0xFC92); 
					else if(cursorX == 1)
						mp.display.fillRect(78, 48, 50, 14, 0xFC92);
				}
				if(mp.buttons.released(BTN_RIGHT) && cursorX < 1)
				{
					blinkState = 0;
					blinkMillis = millis();
					repeat = 1;
					while(!mp.update());
				}
				if(mp.buttons.released(BTN_LEFT) && cursorX > 0)
				{
					blinkState = 0;
					blinkMillis = millis();
					repeat = 0;
					while(!mp.update());
				}
			break;

			case 2:
				if(!blinkState)
					mp.display.fillRect(29 + 14*cursorX, 64, 15, 15, 0xFC92);
				if(mp.buttons.released(BTN_RIGHT) && cursorX < 6)
				{
					cursorX++;
					blinkState = 0;
					blinkMillis = millis();
					while(!mp.update());
				}
				if(mp.buttons.released(BTN_LEFT) && cursorX > 0)
				{
					cursorX--;
					blinkState = 0;
					blinkMillis = millis();
					while(!mp.update());
				}
				if(mp.buttons.released(BTN_A))
				{
					days[cursorX] = !days[cursorX];
					blinkState = 1;
					blinkMillis = millis();
					while(!mp.update());
				}
			break;

			case 3:
				if(!blinkState)
				{
					mp.display.drawRect(20, 98, 120, 20, 0xFC92);
					mp.display.drawRect(19, 97, 122, 22, 0xFC92);
				}
				if(mp.buttons.released(BTN_A))
				{
					while(!mp.update());
					mp.display.setFreeFont(TT1);
					listAudio("/Music", 1);
					int16_t i = 0;
					if(audioCount == 0)
					{
						mp.display.fillScreen(0xFC92);
						mp.display.setCursor(0, mp.display.height()/2 - 16);
						mp.display.setTextFont(2);
						mp.display.printCenter("No audio tracks found!");
						uint32_t tempMillis = millis();
						while(millis() < tempMillis + 2000)
						{
							mp.update();
							if(mp.buttons.pressed(BTN_A) || mp.buttons.pressed(BTN_B))
							{
								while(!mp.buttons.released(BTN_A) && !mp.buttons.released(BTN_B))
									mp.update();
								break;
							}
						}
					}
					else
					{
						// i = audioPlayerMenu("Select alarm:", audioFiles, audioCount);
						// mp.display.setTextColor(TFT_BLACK);
						// if (i >= 0)
						// 	localAlarmTrack = audioFiles[i];
						// uint16_t start = 0;
						// while (localAlarmTrack.indexOf("/", start) != -1)
						// 	start = localAlarmTrack.indexOf("/", start) + 1;
						// parsedAlarmTrack = localAlarmTrack.substring(start);
					}

				}

			break;
		}
		if(mp.buttons.released(BTN_UP) && cursorY>0)
		{
			if (cursorY == 3 && !repeat)
				cursorY--;
			cursorY--;
			cursorX = 0;
			blinkState = 0;
			blinkMillis = millis();
			while(!mp.update());
		}
		if (mp.buttons.released(BTN_DOWN) && cursorY < 4 && enabled)
		{
			if (cursorY == 1 && !repeat)
				cursorY++;
			cursorY++;
			cursorX = 0;
			blinkState = 0;
			blinkMillis = millis();
			while(!mp.update());

		}
		if(mp.buttons.released(BTN_B))
		{
			while(!mp.update());
			mp.display.setTextColor(TFT_BLACK);
			mp.display.setTextSize(1);
			mp.display.setTextFont(2);
			mp.display.drawRect(14, 45, 134, 38, TFT_BLACK);
			mp.display.drawRect(13, 44, 136, 40, TFT_BLACK);
			mp.display.fillRect(15, 46, 132, 36, 0xFC92);
			mp.display.setCursor(47, 48);
			mp.display.printCenter("Save and exit?");
			mp.display.setCursor(47, 61);
			mp.display.printCenter("A: yes    B:cancel");
			while(1)
			{
				if(mp.buttons.released(BTN_B))
				{
					while(!mp.update());
					break;
				}
				if(mp.buttons.released(BTN_A))
				{
					while(!mp.update());
					alarmHours[index] = hours;
					alarmMins[index] = mins;
					alarmEnabled[index] = enabled;
					alarmTrack[index] = localAlarmTrack;
					for(int i = 0; i < 7; i++)
					{
						alarmRepeatDays[index][i] = days[i];
					}
					alarmRepeat[index] = repeat;
					saveAlarms();
					//save RTC and exit
					return;
				}
				while(!mp.update());
			}
		}
		mp.update();
	}
	
}
void clockTimer()
{
	uint8_t hours = 0;
	uint8_t mins = 0;
	uint8_t secs = 0;
	uint8_t cursor = 0;
	uint32_t blinkMillis = millis();
	bool blinkState = 1;
	uint32_t timeMillis;
	uint8_t state = 0;
	String temp = "";
	char key;
	mp.display.setTextColor(TFT_BLACK);
	while (!mp.buttons.released(BTN_B))
	{
		key = mp.buttons.kpdNum.getKey();
		Serial.println(state);
		Serial.println(key);
		Serial.println("-------------");
		delay(5);
		mp.display.fillScreen(0x97F6);
		temp = "";
		if (hours < 10)
			temp.concat("0");
		temp.concat(hours);
		temp.concat(":");
		if (mins < 10)
			temp.concat("0");
		temp.concat(mins);
		temp.concat(":");
		if (secs < 10)
			temp.concat("0");
		temp.concat(secs);
		mp.display.setTextFont(2);
		mp.display.setTextSize(2);
		mp.display.setCursor(15, 25);
		mp.display.printCenter(temp);
		if(millis()-blinkMillis >= 500)
		{
			blinkState = !blinkState;
			blinkMillis = millis();
		}
		
		mp.display.setTextFont(2);
		mp.display.setTextSize(1);
		mp.display.setCursor(123,110);
		switch (state)
		{
			case 0:
				if(key != NO_KEY)
				{
					if(key == 'A' && (secs > 0 || mins > 0 || hours > 0))
					{
						if(secs > 59)
						{
							secs %= 60;
							mins++;
						}
						if(mins > 59)
						{
							mins %= 60;
							hours++;
						}
						timeMillis = millis();
						state = 1;
						break;
					}
					blinkState = 1;
					blinkMillis = millis();
					switch (cursor)
					{
						case 0:
							if(key == 'C')
								secs /= 10;
							else if (key != 'B' && key != 'D' && key != '#' && key != '*' && secs < 10)
								secs = secs * 10 + key - 48;
						break;
						case 1:
							if(key == 'C')
								mins /= 10;
							else if (key != 'B' && key != 'D' && key != '#' && key != '*' && mins < 10)
								mins = mins * 10 + key - 48;
						break;
						case 2:
							if(key == 'C')
								hours /= 10;
							else if (key != 'B' && key != 'D' && key != '#' && key != '*' && hours < 10)
								hours = hours * 10 + key - 48;
						break;
					}
				}
				mp.display.print("Start");
				mp.display.setCursor(2,110);
				mp.display.print("Erase");

				if(mp.buttons.released(BTN_LEFT) && cursor < 2)
				{
					blinkState = 0;
					blinkMillis = millis();
					cursor++;
					while(!mp.update());
				}
				if(mp.buttons.released(BTN_RIGHT) && cursor > 0)
				{
					blinkState = 0;
					blinkMillis = millis();
					cursor--;
					while(!mp.update());
				}
				if(mp.buttons.released(BTN_A) && (secs > 0 || mins > 0 || hours > 0))
				{
					if(secs > 59)
					{
						secs %= 60;
						mins++;
					}
					if(mins > 59)
					{
						mins %= 60;
						hours++;
					}
					state = 1;
					while(!mp.update());
					break;
				}
				if(!blinkState)
				{
					switch (cursor)
					{
						case 0:
							mp.display.fillRect(102, 0, 50, 60, 0x97F6);
						break;

						case 1:
							mp.display.fillRect(64, 0, 33, 60, 0x97F6);
						break;

						case 2:
							mp.display.fillRect(0, 0, 56, 60, 0x97F6);
						break;
					}
				}
				break;
			
			case 1:
				mp.display.print("Pause");
				mp.display.setCursor(2,110);
				mp.display.print("Reset");
				if(millis()-timeMillis >= 1000)
				{
					timeMillis = millis();
					if(secs > 0)
						secs--;
					else
					{
						if(mins == 0 && hours == 0)
						{
							while(!mp.buttons.released(BTN_A) && !mp.buttons.released(BTN_B))
							{
								mp.display.fillRect(0, 64, 160, 100, 0x97F6);
								mp.display.setCursor(70, 85);
								mp.display.printCenter("(press A)");
								mp.display.setCursor(70, 70);
								if(millis()%700 >= 350)
									mp.display.printCenter("DONE!");
								if(millis()%1000 <= 10)
								{
									osc->note(87, 0.4);
									osc->play();
								}
								mp.update();
							}
							osc->stop();
							while(!mp.update());
							state = 0;
							break;
						}
						secs = 59;
						if(mins > 0)
							mins--;
						else
						{
							mins = 59;
							if(hours > 0)
								hours--;
							else
								mins = 0;
						}
					}
				}
				if(mp.buttons.released(BTN_A) || key == 'A')
				{
					state = 2;
					while(!mp.update());
					break;
				}
				if(key == 'C')
				{
					state = 0;
					secs = 0;
					mins = 0;
					hours = 0;
					while(!mp.update());
					break;
				}
				break;
			
			case 2:
				// if(!blinkState)
				// {
				// 	mp.display.fillRect(102, 0, 50, 60, 0x97F6);
				// 	mp.display.fillRect(64, 0, 33, 60, 0x97F6);
				// 	mp.display.fillRect(0, 0, 56, 60, 0x97F6);
				// }
				if(mp.buttons.released(BTN_A) || key == 'A')
				{
					state = 1;
					mp.display.fillRect(0, 64, 160, 100, 0x97F6);
					mp.display.setCursor(123,110);
					mp.display.print("Pause");
					mp.display.setCursor(2,110);
					mp.display.print("Reset");
					while(!mp.update());
					break;
				}
				if(key == 'C')
				{
					state = 0;
					secs = 0;
					mins = 0;
					hours = 0;
					while(!mp.update());
					break;
				}
				mp.display.setCursor(70, 75);
				mp.display.printCenter("paused");
				mp.display.setCursor(114,110);
				mp.display.print("Resume");
				mp.display.setCursor(2,110);
				mp.display.print("Reset");
				break;
		}
		mp.update();
	}
}
void saveAlarms()
{
	const char * path = "/.core/alarms.json";
	Serial.println("");
	mp.SD.remove(path);
	JsonArray& alarms = mp.jb.createArray();

	if (alarms.success()) {
		for(int i = 0; i<5;i++)
		{
			JsonObject& tempAlarm = jb.createObject();
			tempAlarm["hours"] = alarmHours[i];
			tempAlarm["mins"] = alarmMins[i];
			tempAlarm["enabled"] = alarmEnabled[i];
			tempAlarm["repeat"] = alarmRepeat[i];
			JsonArray& days = jb.createArray();
			for(int x = 0; x<7;x++)
			{
				days.add(alarmRepeatDays[i][x]);
			}
			tempAlarm["days"] = days;
			tempAlarm["track"] = alarmTrack[i];		
			alarms.add(tempAlarm);	
		}

		SDAudioFile file1 = mp.SD.open(path, "w");
		alarms.prettyPrintTo(file1);
		alarms.prettyPrintTo(Serial);
		file1.close();
	} else {
		Serial.println("Error saving alarm data");
	}
}
void loadAlarms()
{
	const char * path = "/.core/alarms.json";
	Serial.println(""); 
	SDAudioFile file = mp.SD.open(path);
	JsonArray& alarms = mp.jb.parseArray(file);
	file.close();
	
	if (alarms.success()) {
		int i = 0;
		for(JsonObject& tempAlarm:alarms)
		{
			alarmHours[i] = tempAlarm["hours"];
			alarmMins[i] = tempAlarm["mins"];
			alarmEnabled[i] = tempAlarm["enabled"];
			alarmRepeat[i] = tempAlarm["repeat"];
			JsonArray& days = tempAlarm["days"];
			for(int x = 0; x<7;x++)
			{
				alarmRepeatDays[i][x] = days[x];
			}
			alarmTrack[i] = String(tempAlarm["track"].as<char*>());	
			i++;
		}
	}
	else {
		Serial.println("Error loading new alarms");
	}
	alarms.prettyPrintTo(Serial);
}