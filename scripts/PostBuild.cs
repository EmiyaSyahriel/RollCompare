using System;
using System.IO;
using System.Collections.Generic;

string projectName = Args[0];
string fileType = Args[1];

/* Configuration Format : {folder, suffix} */
Dictionary<string, string> configs = new Dictionary<string, string>() {
    {"x86-Release","r32" },
    {"x86-Debug","d32" },
    {"x64-Release","r64" },
    {"x64-Debug","d64" },
    {"x64-Debug (Default)","d64" } // First config my Visual Studio creates
};


if (!Directory.Exists("bin")) {
    Console.WriteLine("\"bin\" folder not found, creating...");
    Directory.CreateDirectory("bin"); 
}

Console.WriteLine($"Active Project : {projectName} | {fileType}");

foreach(var kvp in configs)
{
    bool proceedCopy = true;
    string srcfile = $"out/build/{kvp.Key}/{projectName}/{projectName}.{fileType}";
    string dstfile = $"bin/{projectName}_{kvp.Value}.{fileType}";

    if (!Directory.Exists($"out/build/{kvp.Key}/{projectName}") || !File.Exists(srcfile))
    {
        proceedCopy = false;
        Console.WriteLine($"Configuration \"{kvp.Key}\" hasn't been built yet");
    }

    if (File.Exists(dstfile))
    {
        if(File.GetCreationTime(dstfile) == File.GetCreationTime(srcfile))
        {
            proceedCopy = false;
            Console.WriteLine($"Configuration \"{kvp.Key}\" build is up-to-date.");
        }
    }

    if (proceedCopy)
    {
        File.Copy(srcfile, dstfile, true);
        Console.WriteLine($"Configuration \"{kvp.Key}\" build has been copied.");
    }
}