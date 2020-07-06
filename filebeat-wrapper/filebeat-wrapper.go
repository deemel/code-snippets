package main

import (
	"io/ioutil"
	"log"
	"os"
	"strings"
	"syscall"
)

const EnvVarAdpConf = "ADP_CONF"
const EnvVarFbRoot = "FB_ROOT"
const EnvVarFbConf = "FB_CONF"
const EnvVarFbYml = "FB_YML"
const EnvVarFbCmdLineFlags = "FB_CMD_LINE_FLAGS"
const EnvVarLogstashHosts = "LOGSTASH_HOSTS"
const DefAdpConf = "/opt/adp/conf"
const DefFbRoot = "/opt/filebeat"
const DefFbCmdLineFlags = "-e -path.config "
const FilenameFbConf = "filebeat.conf"
const FilenameFbTemplateLog = "filebeat.prospectors.log.template"
const FilenameFbTemplate = "filebeat.yml.template"
const FilenameFbYml = "filebeat.yml"
const ConfPaths = "paths:"
const ConfFields = "fields:"
const ConfFieldsFormatted = "  " + ConfFields + "\n"
const ConfLogplane = "logplane:"
const ConfLogplaneDef = ConfLogplane + " '${FILEBEAT_PROSPECTORS_LOG_FIELDS_LOGPLANE}'"
const ConfLogplaneDefFormatted = "    " + ConfLogplaneDef + "\n"
const KeyFbConf = "# FILEBEAT_CONF\n"
const KeyFbProspectorsLog = "# FILEBEAT_PROSPECTORS_LOG\n"
const PermWorldRead = 0644
const CmdFb = "filebeat"

func Getenv(key, valueDef string) string {
	value := os.Getenv(key)
	if value == "" {
		if valueDef == "" {
			log.Fatal("Environment variable <" + key + "> is not set.  No default value provided.")
		}

		log.Print("Environment variable <" + key + "> is not set.  Using default value <" + valueDef + ">.")
		return valueDef
	}

	return value
}

func ReadFile(filename string) string {
	bytes, err := ioutil.ReadFile(filename)
	if err != nil {
		log.Fatal(err)
	}

	return string(bytes)
}

func NewFbConf(conf string, isFoundFields, isFoundLogplane bool) string {
	if isFoundFields {
		if !isFoundLogplane {
			conf += ConfLogplaneDefFormatted
		}
	} else {
		conf += ConfFieldsFormatted
		conf += ConfLogplaneDefFormatted
	}

	return conf
}

func StartFb(dirFbRoot, CmdFb, fbCmdLineFlags string) {
	binary := dirFbRoot + "/" + CmdFb
	args := strings.Split((CmdFb + " " + fbCmdLineFlags), " ")

	log.Print("Starting Filebeat with command <" + binary + " " + fbCmdLineFlags + ">.")

	err := syscall.Exec(binary, args, os.Environ())
	if err != nil {
		log.Fatal(err)
	}
}

func main() {
	dirAdpConf := Getenv(EnvVarAdpConf, DefAdpConf)
	dirFbRoot := Getenv(EnvVarFbRoot, DefFbRoot)
	fbCmdLineFlags := Getenv(EnvVarFbCmdLineFlags, (DefFbCmdLineFlags + dirFbRoot))
	pathFbYml := dirFbRoot + "/" + FilenameFbYml
	fbYmlUser := Getenv(EnvVarFbYml, (dirAdpConf + "/" + FilenameFbYml))

	_, err := os.Stat(fbYmlUser)
	if err == nil {
		bytesFbYmlUser, err := ioutil.ReadFile(fbYmlUser)
		if err != nil {
			log.Fatal(err)
		}

		err = ioutil.WriteFile(pathFbYml, bytesFbYmlUser, PermWorldRead)
		if err != nil {
			log.Fatal(err)
		}

		log.Print("File <" + pathFbYml + "> copied from user-provided configuration <" + fbYmlUser + ">.")

		StartFb(dirFbRoot, CmdFb, fbCmdLineFlags)
	}

	_ = Getenv(EnvVarLogstashHosts, "")
	fbConf := Getenv(EnvVarFbConf, (dirAdpConf + "/" + FilenameFbConf))

	strFbTemplate := ReadFile(dirAdpConf + "/" + FilenameFbTemplate)
	strFbTemplateLog := ReadFile(dirAdpConf + "/" + FilenameFbTemplateLog)
	strFbConf := ReadFile(fbConf)

	conf := ""
	confLogs := ""
	isFoundPaths := false
	isFoundFields := false
	isFoundLogplane := false

	for _, i := range strings.Split(strFbConf, "\n") {
		i = strings.TrimSpace(i)
		if i == ConfPaths {
			if isFoundPaths {
				conf = NewFbConf(conf, isFoundFields, isFoundLogplane)
				confLogs += (strings.Replace(strFbTemplateLog, KeyFbConf, conf, -1) + "\n")

				conf = ""
				isFoundPaths = false
				isFoundFields = false
				isFoundLogplane = false
			}

			isFoundPaths = true

			conf += ("  " + i + "\n")
			continue
		} else if i == ConfFields {
			isFoundFields = true

			conf += ("  " + i + "\n")
			continue
		} else if strings.HasPrefix(i, ConfLogplane) {
			isFoundLogplane = true
		} else if i == "" {
			continue
		}

		conf += ("    " + i + "\n")
	}

	if isFoundPaths {
		conf = NewFbConf(conf, isFoundFields, isFoundLogplane)
		confLogs += strings.Replace(strFbTemplateLog, KeyFbConf, conf, -1)
	}

	confFb := strings.Replace(strFbTemplate, KeyFbProspectorsLog, confLogs, -1)

	err = ioutil.WriteFile(pathFbYml, []byte(confFb), PermWorldRead)
	if err != nil {
		log.Fatal(err)
	}

	log.Print("File <" + pathFbYml + "> generated from user-provided configuration <" + fbConf + "> ...\n" + confFb)

	StartFb(dirFbRoot, CmdFb, fbCmdLineFlags)
}
