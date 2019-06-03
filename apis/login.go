package apis

import (
	"DBexp/model"
	"DBexp/utils"
	"log"
	"net/http"
)


func Login(w http.ResponseWriter, r *http.Request){
	defer utils.RecoverResolve("apis.Login")
	retMap := make(map[string]interface{})

	if r.Method != "POST"{
		log.Printf("apis.Login r.Method not match, method=%v",r.Method)
		retMap["code"] = utils.METHODERR
		utils.SendJson(&retMap,w)
		return
	}

	resq := utils.Json2Map(r)
	_uid := resq["uid"]
	if _uid == nil{
		log.Printf("apis.Login no resq value")
		retMap["code"] = utils.PARAMERR
		utils.SendJson(&retMap,w)
		return
	}
	uid := (int)(_uid.(float64))

	login,uidType,err := model.VerifyUid(uid)
	if err != nil{
		log.Printf("apis.Login VerifyUid error, err=%v",err)
		retMap["code"] = utils.PARAMERR
		utils.SendJson(&retMap,w)
		return
	}
	if !login{
		retMap["code"] = utils.NOTLOGIN
		utils.SendJson(&retMap,w)
		return
	}
	token, err :=model.GetTokenByUid(uid)
	if err != nil{
		log.Printf("GetTokenByUid error, err=%v",err)
		retMap["code"] = utils.PARAMERR
		return
	}
	retMap["code"] = 0
	retMap["token"] = token
	retMap["type"] = uidType
	utils.SendJson(&retMap,w)
	return

}

func SignIn(w http.ResponseWriter, r *http.Request){
	defer utils.RecoverResolve("apis.Login")
	retMap := make(map[string]interface{})

	if r.Method != "POST"{
		retMap["code"] = utils.METHODERR
		utils.SendJson(&retMap,w)
		return
	}

	resq 	:= 		utils.Json2Map(r)
	_uid 	:= 		resq["uid"]
	_name 	:= 		resq["name"]
	_type 	:= 		resq["type"]
	if _uid == nil || _name == nil || _type==nil{
		log.Printf("apis.SignIn no resq value")
		retMap["code"] = utils.PARAMERR
		utils.SendJson(&retMap,w)
		return
	}

	uid		:= 		(int)(_uid.(float64))
	name 	:= 		_name.(string)
	ttype 	:= 		(int)(_type.(float64))
	err 	:= 		model.SignIn(uid,name,ttype)
	if err != nil{
		log.Printf("apis.SignIn model.SignIn error, err=%v",err)
		retMap["code"] = utils.DATABASEERR
		utils.SendJson(&retMap,w)
		return
	}
	retMap["code"] = 0
	retMap["type"] = ttype
	retMap["name"] = name
	utils.SendJson(&retMap, w)
	return
}







