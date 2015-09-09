#!/bin/bash -e

if [ "$#" != "3" ]; then
cat <<EOM
Usage:
    $0 <game> <language> <language>

Where:
    <game> one of: $(ls /Mjollnir/vigridr/src/games/ | grep -v json | xargs)
    <language> one of: cpp cs java py
EOM
exit 1
fi

echo "Changing game code"
cd /Mjollnir/vigridr/src/
python change_game_code.py $1 --with-client 1>/dev/null

echo "Creating game folder"
rm -rf ~/game
mkdir ~/game
mkdir ~/game/server
mkdir ~/game/client1
mkdir ~/game/client2

cd /Mjollnir/vigridr/
cp src/games/$1/bin/server ~/game/server || (echo "/Mjollnir/vigridr/src/games/$1/bin/server does not exist. Did build all games?" && exit 1)
echo "Copied server"

client() {
    case "$1" in
        cpp)
            make clientcpp &>/dev/null
            cp bin/cpp/* ~/game/$2
            rm ~/game/$2/server
        ;;
        cs)
            make clientcsharp &>/dev/null
            cp bin/csharp/* ~/game/$2
        ;;
        java)
            make clientjava #&>/dev/null
            cp bin/java/* ~/game/$2
        ;;
        py)
            make clientpy &>/dev/null
            cp -r bin/py/* ~/game/$2
        ;;
        *)
            echo "Wrong language"
            exit 1
        ;;
    esac
    echo "Built and copied $2"
}

client $2 client1
client $3 client2

echo "Starting terminals"

gnome-terminal -t Server --working-directory="$HOME/game/server" -x bash -c 'read -p "Press enter to run server" && ./server && echo -e "\nSee logs at ~/game/server/logs.\nPress enter to exit" && read'
gnome-terminal -t Client1 --working-directory="$HOME/game/client1" -x bash -c 'read -p "Press enter to run client1" && ./client --port 9090 && read -p "Press enter to exit"'
gnome-terminal -t Client2 --working-directory="$HOME/game/client2" -x bash -c 'read -p "Press enter to run client2" && ./client --port 9091 && read -p "Press enter to exit"'

echo "Done"

