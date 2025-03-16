{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "g++ build active file",
            "type": "shell",
            "command": "/usr/bin/g++",
            "args": [
                "-g",
                "${workspaceFolder}/src/main.cpp",
                "${workspaceFolder}/src/Cmd.cpp",
                "${workspaceFolder}/src/Command.cpp",
                "${workspaceFolder}/src/EthernetFrame.cpp",
                "${workspaceFolder}/src/RxObserver.cpp",
                "${workspaceFolder}/src/COM.cpp",
                "-o",
                "${workspaceFolder}/cmd",
                "-I${workspaceFolder}/include",
                "-I/usr/include/jsoncpp",   // Corrected JSON include path
                "-L/usr/lib",               // Add this if needed
                "-lnet",
                "-lpcap",
                "-lpthread",
                "-ljsoncpp"
            ],
            "group": {
                "kind": "build",
                "isDefault": true
            },
            "problemMatcher": ["$gcc"],
            "detail": "Generated task to build the project"
        }
    ]
}
