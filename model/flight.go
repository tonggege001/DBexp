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

//获取预定了的
func GetSiteBookIDByFid(fid int) []int{
	tx := MysqlDB
	retList := make([]int ,0)
	rows, err := tx.Query("select distinct siteid from flight_site where fid = ?",fid)
	if err != nil{
		log.Printf("GetSiteBookIDByFid tx.Query error, err=%v", err)
		return retList
	}
	defer rows.Close()

	for rows.Next(){
		sitNum := -1
		err := rows.Scan(&sitNum)
		if err != nil{
			log.Printf("GetSiteBookIDByFid Scan Error, err=%v",err)
			return retList
		}
		retList = append(retList, sitNum)
	}
	return retList
}

func AddFlightInfo(fid int, ftype string, cityFrom string, cityTo string,timeBegin time.Time,
timeEnd time.Time, food bool, sitnum int, income float64)(int,error){
	tx := MysqlDB
	stmt, err := tx.Prepare("insert into flight_info(fid, ftype, city_from, city_to, time_begin, time_end, food, sitenum, income) VALUES (?,?,?,?,?,?,?,?,?)")
	if err != nil{
		log.Printf("AddFlightInfo tx.Prepare error, err=%v",err)
		return 0, err
	}

	_, err = stmt.Exec(fid,ftype,cityFrom,cityTo,timeBegin,timeEnd,food,sitnum,income)
	if err != nil{
		log.Printf("AddFlightInfo stmt.Exec error, err=%v",err)
		return 0,err
	}
	defer stmt.Close()

	return fid,nil
}

func ChangeFlightInfo(fid int, ftype string, cityFrom string, cityTo string,timeBegin time.Time,
	timeEnd time.Time, food bool, sitnum int, income float64)error{

	tx := MysqlDB

	stmt,err := tx.Prepare("update flight_info set ftype=?, city_from=?, city_to=?,time_begin=?,time_end=?,food=?,sitenum=?,income=? where fid = ?")
	if err != nil{
		log.Printf("ChangeFlightInfo tx.Prepare error, err=%v",err)
		return err
	}

	_, err = stmt.Exec(ftype, cityFrom, cityTo, timeBegin, timeEnd, food, sitnum, income,fid)
	defer stmt.Close()
	if err != nil{
		log.Printf("ChangeFlightInfo stmt.Exec error, err=%v",err)
		return err
	}
	return nil

}














