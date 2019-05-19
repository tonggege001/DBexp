package utils

import "log"

func RecoverResolve(errstr string){
	if err:=recover();err!=nil{
		log.Printf(errstr+" A panic happened!")
		log.Println(err) // 这里的err其实就是panic传入的内容，55
	}

}
