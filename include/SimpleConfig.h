
/******************************************************************
Management of a very simple XML 
Gestion d'un XML simplifie avec des valeurs int, double, ...
Done by Francois Braud
******************************************************************/

#ifndef SimpleConfig_h_INCLUDED
#define SimpleConfig_h_INCLUDED

#include <map>
#include <string>


/**
* \brief A very simple XML config file loader/store/saver

Example of XML file which works:
<pre>

<root>
<string>
<windowname>window name</windowname>
</string>
<double>
</double>
<int>
<windowwidth>1024</windowwidth>
<windowheight>768</windowheight>
</int>
<bool>
</bool>
</root>

</pre>
*/

class SimpleConfig
{
public:

    //! Create a parser
    //! Cree un parseur
    SimpleConfig();

	//! Load data from a file
    //! Charge les donnees a partir d'un fichier
    void loadFile(const char *fileName, bool parFailIfDoNotExist);

	//! Save data in a file
    //! Sauvegarde les donnees dans un fichier
    void saveFile(const char* fileName) const;

    //! Get a string item value
    //! Recupere la valeur d'un item contenant une chaine de caractere
    std::string getStringValue(std::string name){return itemsString[name];}

    //! Get a double item value
    //! Recupere la valeur d'un item contenant un double
    double getDoubleValue(std::string name){return itemsDouble[name];}

    //! Get a double item value
    //! Recupere la valeur d'un item contenant un double
    float getDoubleValueAsFloat(std::string name){return (float)itemsDouble[name];}
	float getFloatValue(std::string name){return (float)itemsDouble[name];}

    //! Get a integer item value
    //! Recupere la valeur d'un item contenant un int
    int getIntValue(std::string name){return itemsInt[name];}

    //! Get a bool item value
    //! Recupere la valeur d'un item contenant un bool
    bool getBoolValue(std::string name){return itemsBool[name];}

	//! Set new values to items
    //! Affecte de nouvelles valeurs aux items
    void setStringValue(std::string name, std::string value) {itemsString[name] = value;}
    void setDoubleValue(std::string name, double value) {itemsDouble[name] = value;}
    void setIntValue(std::string name, int value) {itemsInt[name] = value;}
    void setBoolValue(std::string name, bool value) {itemsBool[name] = value;}

    //! Print all the items and their values on the console
    //! Affiche tous les items du fichier xml dans la console
    void print();

	//! Get items values
    //! Recupere les items
    const std::map<std::string,std::string>& getItemsString() {return itemsString;}
    const std::map<std::string,double>& getItemsDouble() {return itemsDouble;}
    const std::map<std::string,int>& getItemsInt() {return itemsInt;}

private:
    std::map<std::string, std::string> itemsString;
    std::map<std::string, double> itemsDouble;
    std::map<std::string, int> itemsInt;
    std::map<std::string, bool> itemsBool;
};

#endif //SimpleConfig_h_INCLUDED
