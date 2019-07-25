# sh targets/debugger/build.sh

SERVICE_NAME=./nes
# stop
P_ID=`ps -ef | grep -w "$SERVICE_NAME" | grep -v "grep" | awk '{print $2}'`
if [ -z "$P_ID" ]; then
  echo "======= $SERVER_NAME process not exits or stop success"
else
  echo "======= $SERVER_NAME process pid is:$P_ID"
  echo "======= begin kill $SERVER_NAME process, pid is :$P_ID"
  kill -9 $P_ID
fi

# sleep 2

# # start
# `${SERVICE_NAME}`
# # nohup `${SERVICE_NAME}` >/dev/null 2>&1 &