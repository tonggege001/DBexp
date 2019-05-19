package model

import (
	"errors"
	"fmt"
)

//验证是否存在该用户
func VerifyUid(uid int)(bool, error){
	tx := MysqlDB

	rows,err := tx.Query("select uid from user_info where uid=?",uid)
	if err != nil{
		//TODO 修改成查询失败的处理
		return false,errors.New("VerifyUid Query error")
	}
	defer rows.Close()

	uidDb := 0

	for rows.Next(){
		err := rows.Scan(&uidDb)
		if err != nil{
			return false,errors.New(fmt.Sprintf("VerifyUid uid scan error, err = %v", err))
		}
		break
	}
	if uidDb != uid {
		return false,nil
	}

	return true,nil
}

func GetTokenByUid(uid int)(string, error){
	tx := MysqlDB

	rows,err := tx.Query("select token from user_login where uid=?", uid)
	if err != nil{
		return "",errors.New("GetTokenByUid Query error")
	}

	tokenStr := ""
	defer rows.Close()
	for rows.Next(){
		err := rows.Scan(&tokenStr)
		if err != nil{
			return "", errors.New("GetTokenByUid uid scan error")
		}
		break
	}
	if tokenStr != ""{
		return tokenStr, nil
	}else{
		//插入新的字符串
		_, err := tx.Query("insert into user_login(uid, token) values (?,?)",uid,string(uid))
		if err != nil{
			return "", err
		}else{
			return string(uid), nil
		}
	}
}

func GetUidByToken(token string)(int,error){
	tx := MysqlDB
	rows, err := tx.Query("select uid from user_login where token=?", token)
	if err != nil{
		return 0,err
	}

	uid := 0
	for rows.Next(){
		err := rows.Scan(&uid)
		if err != nil{
			return 0,err
		}
	}

	if uid == 0{
		return uid,errors.New("token map error")
	}
	return uid,nil
}










