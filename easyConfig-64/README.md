# Game Switcher for SpruceOS on Miyoo A30

EasyConfig is a SDL2 program run on Miyoo A30 game console. It is used for configurating Game Switcher and other settings.

```
Usage: easyConfig config_file [-t title] [-p index] [-o]

-t:     title of the config window.
-p:     show i-th group only (first group index = 1).
-o:     generate options only
-h,--help       show this help message.

UI control: L1/R1: Select group, Up/Down: Select item, Left/Right/A: Change value, B: Save and exit, Select: Cancel and exit

The config file should contains lines of config settings, in the following format:
"NAME" "DESCRIPTION" "POSSIBLE_VALUES" "DISPLAY_VALUES" "CURRENT_VALUE" ["COMMANDS"] ["UPDATE_COMMAND"]

NAME: short name used in options file.
DESCRIPTION: description shown in config window.
POSSIBLE_VALUES: possible values of setting.
DISPLAY_VALUES: corresponding display texts of possible values shown in config window.
CURRENT_VALUES: current value of setting, should be one of the display values.
COMMANDS: optional commands to be executed on exit if the setting value is changed.
UPDATE_COMMAND: optional command to be executed to update the minor text on setting value is changed.

POSSIBLE_VALUES, DISPLAY_VALUES and COMMANDS are values seperated by '|'. Example lines of config file:

"-s" "Text scrolling speed" "10|20|30" "Slow|Normal|Fast" "Fast"
"-t" "Display title at start" "on|off" "on|off" "on"

Minor text can be added to a setting item to show additional information to user. To define a minor text insert line after a setting item with the following format:

@"This is minor text"

If there is only one command in COMMANDS field, this will be general command and will be executed everytime when the setting value is changed.

If UPDATE_COMMAND exists, it will be executed when the corresponding setting value is changed by user. The command should print new information to stdout to update the minor text.

You can define alias in the begining of the config file and use them to represent long commands in fields CURRENT_VALUE, COMMANDS and UPDATE_COMMAND. To define an alias insert line with the following format:

$ALIAS_NAME=long string to replaced$

And to use the alias in a command replace the string to be replaced with the syntex $ALIAS_NAME$. Also you can use predefined alias _VALUE_ and _INDEX_ to represent the current selected value and its zero-based index of the setting item.

Dynamic information text can be added as single item, which should be a command to be executed on start. The command should print output to stdout to set the information text. To define an information text add a line with the following format

%"the_commond_to_run_on_start.sh"

Setting items can be organized into groups, which displayed as multiple tags in config window. To define a group insert line with the following format:

[GROUP_NAME] [OPTIONAL_OUTPUT_FILENAME]

Note that the square brackets are part of input. Any setting items after the group definition will be assigned to the group. OPTIONAL_OUTPUT_FILENAME is the filename of optional output file. Output option file is generated when program exit, which containing pairs of NAME and VALUE, can be utilized as option list for calling another program. Example output options:

-s 10 -b off -t on -ts 4 -n on

Config file is updated with new values when program exit.
```

# Links
Original repositories
https://github.com/oscarkcau/easy-config-A30
https://github.com/oscarkcau/game-switcher-A30

SpruceOS
https://github.com/spruceUI/spruceOS

Miyoo webpage
https://www.lomiyoo.com/
