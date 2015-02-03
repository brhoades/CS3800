#!/bin/bash
# Name: Billy Rhoades
# Class: CS3800 Section B

function users
{ 
  w -h | cut -f1 -d' ' | sort -u 
}

while [[ "$SELECTION" != "Exit" ]]
do
  PS3="Choose an option: "
  [[ "$SELECTION" != "" ]] && printf "\n\n"                    # Space if this isn't our first run
  
  select SELECTION in "Print Ancestry Tree" "List Users Online" "Get User's Processes" 'Exit'
    do
      case $SELECTION in 
        "Print Ancestry Tree")
          OUTPUT=$(pstree --ascii -ps $$)                      # Separate or it gets greedy and grabs sed
          echo $OUTPUT | sed -e 's/---/\n|\n/g' | head -n -2   # Format it into a tree
          break;;

        "List Users Online")
          echo Users Online:
          echo $(users)
          break;;

        "Get User's Processes")
          printf "\n"
          USERLIST=$(users)
          PS3="Choose a user: " 
          select USERNAME in $USERLIST
          do
            ps -U $USERNAME l
            break
          done
          break;;

        'Exit')
          break;;

        * )
          echo "Unknown Option"
          break;;
    esac
  done
done
