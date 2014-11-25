// Import the DSS packages into our namespace to save on typing
importPackage(Packages.com.ti.debug.engine.scripting)
importPackage(Packages.com.ti.ccstudio.scripting.environment)
importPackage(Packages.java.lang)


// the workspace directory
ws = environment['osgi.instance.area']

print(ws);

// Configurable Parameters
var deviceCCXMLFile = "./targetConfigs/MSP430FR5969.ccxml";
var programToLoad = "./Debug/RIOT.out";

// Create our scripting environment object - which is the main entry point into any script and
// the factory for creating other Scriptable ervers and Sessions
var script = ScriptingEnvironment.instance();

// Create a debug server
var ds = script.getServer( "DebugServer.1" );

// Set the device ccxml 
ds.setConfig( deviceCCXMLFile );

// Open a debug session
debugSession = ds.openSession( ".*" );

debugSession.target.connect();

// Load the program 
debugSession.memory.loadProgram( programToLoad );

pc = debugSession.memory.readRegister('PC');

while(true) {
	print(pc.toString(16));
	debugSession.target.contextStep.into();
	pc = debugSession.memory.readRegister('PC');
}



