#include <aJSON.h>

void setup()
{
  char* data = "{\"EditorialWorkload\": \"51\", \"DesignWorkload\": \"96\", \"AccountManagementWorkload\": \"21\"}";
  aJsonObject* jsonObject = aJson.parse(data);
  aJsonObject* ed = aJson.getObjectItem(jsonObject, "EditorialWorkload");
  Serial.begin(9600);
  Serial.println("Editorial:");
  Serial.println(ed->valuestring);
}

void loop()
{

}

