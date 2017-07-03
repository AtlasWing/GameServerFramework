#!/bin/sh

#日志监视脚本
#监视zoneserver ./log.sh zo
#监视dataServer ./log.sh da
#监视logicServer ./log.sh lo
#监视gatewayServer ./log.sh ga
#监视所有日志 ./log.sh

PARA=$1
id=$2
CONFIG_DIR=$3
if [ "$CONFIG_DIR" = "" ]
then
    CONFIG_DIR="config"
fi
CONFIG="$CONFIG_DIR/ZoneConfig.xml"
ADMIN_CONGIG="adminServer/adminConfig.xml"
main()
{
	ZONELOG=`grep zoneserver.log $CONFIG |sed -e 's/<LOG_FILE>//'|sed -e 's/<\/LOG_FILE>//'`
	DATALOG=`grep dataserver.log $CONFIG |sed -e 's/<LOG_FILE>//'|sed -e 's/<\/LOG_FILE>//'`
	LGLOG=`grep logicserver.log $CONFIG |sed -e 's/<LOG_FILE>//'|sed -e 's/<\/LOG_FILE>//'`
	GATEWAYLOG=`grep gatewayserver.log $CONFIG |sed -e 's/<LOG_FILE>//'|sed -e 's/<\/LOG_FILE>//'`
	ADMINLOG=`grep adminserver.log $ADMIN_CONGIG |sed -e 's/<LOG_FILE>//'|sed -e 's/<\/LOG_FILE>//'`
	ALL=$ZONELOG" "$LGLOG" "$DATALOG" "$GATEWAYLOG" "$ADMINLOG
	
	clear
	case $PARA in 
		zo)
		tail --follow=name --retry $ZONELOG --max-unchanged-stats=3 -n 40 -q
		;;
		lo)
        if [ "$id" = "" ]
        then
		    tail --follow=name --retry $LGLOG  --max-unchanged-stats=3 -n 40 -q
        else
            tail --follow=name --retry $LGLOG --max-unchanged-stats=3 -n 40 -q | grep "LO $id"
        fi
        ;;
		da)
		tail --follow=name --retry $DATALOG --max-unchanged-stats=3 -n 40 -q
		;;
		ga)
        if [ "$id" = "" ]
        then
		    tail --follow=name --retry $GATEWAYLOG  --max-unchanged-stats=3 -n 40 -q
        else
		    tail --follow=name --retry $GATEWAYLOG  --max-unchanged-stats=3 -n 40 -q | grep "GA $id"
	    fi
        ;;
        pr)
		tail --follow=name --retry $PRESSURELOG --max-unchanged-stats=3 -n 40 -q
        ;;
		*)
		tail --follow=name --retry $ALL --max-unchanged-stats=3 -n 5 -q 
		;;
	esac
}
main
