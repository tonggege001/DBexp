package utils

import (
	"encoding/json"
	"log"
	"net/http"
)

func JSON(data *map[string]interface{}) []byte{
	m := make(map[string]interface{})
	m["code"]= 10
	if data == nil{
		ret,_ :=json.Marshal(m)
		return ret
	}

	ret,err := json.Marshal(*data)
	if err!= nil{
		log.Printf("JSON marshall err, data=%v, err=%v",*data, err)
	}
	return ret
}

func Json2Map(r *http.Request) map[string]interface{}{
	defer RecoverResolve("Json2Map")
	m := make(map[string]interface{})
	data := make([]byte,0)
	_, err := r.Body.Read(data)
	if err != nil{
		log.Printf("Json2Map r.Body.Read error, err=%v",err)
		return m
	}
	defer r.Body.Close()
	err = json.Unmarshal(data, &m)
	if err != nil{
		log.Printf("Json2Map Unmarshal error, err=%v, data=%v",err,data)
		return m
	}
	return m
}


func SendJson(data * map[string]interface{}, w http.ResponseWriter){
	bytes :=JSON(data)
	_, err := w.Write(bytes)
	if err != nil{
		log.Printf("SendJson error, err=%v",err)
	}
	return
}
