#include <aJSON.h>

void setup()
{
  char* data = "{EditorialWorkload: 103, DesignWorkload: 105, AccountManagementWorkload: 95}";
  aJsonObject* jsonObject = aJson.parse(data);
  aJsonObject* ed = aJson.getObjectItem(jsonObject, "EditorialWorkload");
  Serial.println(ed->valueint);
}

void loop()
{
  
}
