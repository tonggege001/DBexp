package model

import (
	"log"
	"time"
)

type FlightInfo struct {
	Fid				int
	FType			string
	CityFrom		string
	CityTo			string
	TimeBegin		time.Time
	TimeEnd			time.Time
	Food			bool
	SiteNum			int
	Income			float64
}


func GetFlightInfoByTimeFromTo(btime time.Time,etime time.Time, from string, to string)[]FlightInfo{
	tx := MysqlDB
	retList := make([]FlightInfo,0)
	rows, err := tx.Query("select fid,ftype,time_begin,time_end,city_from,city_to,food,sitenum,income from flight_info where time_begin > ? and time_end < ? and city_from like ? and city_to like ?",btime,etime,"%"+from+"%","%"+to+"%")
	if err != nil{
		log.Printf("GetFlightInfoByTimeFromTo error, err=%v",err)
		return retList
	}
	defer rows.Close()
	for rows.Next(){
		f := FlightInfo{}
		err := rows.Scan(&f.Fid, &f.FType, &f.TimeBegin, &f.TimeEnd, &f.CityFrom, &f.CityTo, &f.Food, &f.SiteNum, &f.Income)
		if err != nil{
			log.Printf("GetFlightInfoByTimeFromTo row Scan error, err=%v", err)
			continue
		}
		retList = append(retList, f)
	}
	return retList
}

func GetSiteBookIDByFid(fid int) []int{
	tx := MysqlDB
	retList := make([]int ,0)
	rows, err := tx.Query("select siteid from flight_site where fid = ?",fid)
	if err != nil{
		log.Printf("GetSiteBookIDByFid tx.Query error, err=%v", err)
		return retList
	}
	defer rows.Close()


}










