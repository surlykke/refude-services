JsonDocument doc(JSON_OBJECT);
/**
 *	{
 *		"_links" : {
 *			"self" : {
 *				"href" : "/bla/bla/bla",
 *				"profile": "http//org.restfulipc.Power/Power"
 *			},
 *			"related" : [
 *				{
 *					"href" : "/bla/bla/bla",
 *					"profile" : "http://org.restfulipc.Power/Battery"
 *				}
 *			]
 *	}
 */

doc.addObjectAndMoveTo("_links")
doc.addObjectAndMoveTo("self")
doc.addString("href", "/bla/bla/bla")
doc.addString("profile", "http://org.restfulipc.Power/Power");
doc.moveUp();
doc.addArrayAndMoveTo("related");
doc.addObject();
doc.addString("href", "/bla/bla/bla");
doc.addString("profile", "http://org.restfulipc.Power/Battery");
doc.moveUp();
