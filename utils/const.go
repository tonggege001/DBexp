package utils

import (
	"math/rand"
	"time"
)

const OK = 0
const NOTLOGIN = 1
const FORMPARSERR  = 2
const PARAMERR = 3
const CODEERR  = 4
const REDISERR = 5
const APICALLERR = 6
const MATHERR  = 7
const CALLERR = 8
const DATABASEERR  = 9
const METHODERR = 10


func GenerateRandom() int{
	rand.Seed(time.Now().Unix())
	return rand.Int()
}

