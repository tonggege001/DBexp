package utils

import (
	"strconv"
	"time"
)

func GetTimeByStrSec(year string, month string, day string, hour string, minut string, second string) (time.Time, error){
	if len(month) == 1{
		month = "0"+month
	}

	if len(day) == 1{
		day = "0"+day
	}

	if len(hour) == 1{
		hour = "0"+day
	}

	if len(minut) == 1{
		minut = "0"+minut
	}
	if len(second) == 1{
		second = "0"+second
	}
	strTime := year+"-"+month+"-"+day+" "+hour+":"+minut+":"+second
	loc, err := time.LoadLocation("Local")
	if err != nil {
		return time.Time{}, err
	}

	theTime, err := time.ParseInLocation("2006-01-02 15:04:05", strTime, loc)
	if err != nil{
		return time.Time{}, err
	}

	return theTime,nil
}

func GetTimeByStr(str string) (time.Time, error){
	for i := len(str);i<12;i++{
		str = str+"0"
	}
	year := str[0:4]
	month := str[4:6]
	day := str[6:8]
	hour := str[8:10]
	minute := str[10:12]
	second := str[12:14]

	t, err := GetTimeByStrSec(year,month,day,hour,minute,second)
	if err != nil{
		return time.Time{}, err
	}
	return t,nil
}

func GetTimeStrByTime(t time.Time)string{
	yy := t.Year()
	yearStr := strconv.Itoa(yy)

	mm := t.Month()
	monthStr := ""
	if mm<10{
		monthStr = "0"+strconv.Itoa(int(mm))
	}else{
		monthStr = strconv.Itoa(int(mm))
	}

	dd := t.Day()
	dayStr := ""
	if dd < 10{
		dayStr = "0"+strconv.Itoa(dd)
	}else{
		dayStr = strconv.Itoa(dd)
	}

	hh := t.Hour()
	hourStr := ""
	if hh < 10{
		hourStr = "0"+strconv.Itoa(hh)
	}else{
		hourStr = strconv.Itoa(hh)
	}

	mi := t.Minute()
	minuteStr := ""
	if mi < 10{
		minuteStr = "0"+ strconv.Itoa(mi)
	}else{
		minuteStr = strconv.Itoa(mi)
	}

	ss := t.Second()
	secondStr := ""
	if ss<10{
		secondStr = "0"+strconv.Itoa(ss)
	}else{
		secondStr = strconv.Itoa(ss)
	}
	return yearStr+monthStr+dayStr+hourStr+minuteStr+secondStr
}



