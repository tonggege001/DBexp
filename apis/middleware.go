package apis

import (
	"TicketBook/model"
	"TicketBook/utils"
	"log"
	"net/http"
)

func TokenCheck(r *http.Request) (bool,int,map[string]interface{}){
	retMap := make(map[string]interface{})
	resq := utils.Json2Map(r)
	if resq["token"] == nil{
		retMap["code"] = utils.NOTLOGIN
		return false, 0, nil
	}

	uid, err := model.GetUidByToken(resq["token"].(string))
	if err != nil{
		log.Printf("TokenCheck GetUidByToken err, err=%v", err)
		retMap["token"] = utils.NOTLOGIN
		return false, 0, nil
	}

	return true,uid,resq
}




