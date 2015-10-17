/******************************************************************
Un champ de texte editable
Done by Francois Braud
******************************************************************/
#ifndef EDITTEXT_HPP
#define EDITTEXT_HPP

#include <iostream>
#include <string>

//---------------------------------------------------------------------

#include "CoreUtils.h"

//---------------------------------------------------------------------

class EditText
{
public:

    //! constructeur
    //! si font == NULL alors utilise une font par defaut
    EditText(bool isUrl,
        int parNbMaxChars = -1,
        const Int2& parPosition = Int2(0,0),
        //const Font* parFont = NULL,
        bool parIsCrypted = false,
        bool parIsDigitOnly = false,
		const std::string& parAllowedCharacters = "");

    //! recupere les evenements clavier
    void injectKeyEvent(unsigned char key);

    //! affiche l'EditText
    void draw(float fontScale = 18.0f);

    //! renvoie le texte (pas une copie !)
    inline const char *getText() const {return text.c_str();}

	void setText(const std::string& newtext) { text = newtext;if (keypos > (int)newtext.size()) keypos = newtext.size(); }

    //! insere un caractere (a la position courante du curseur)
    void insertChar(char c);

    //! insere une chaine (a la fin)
    void insertStringAtEnd(const char *str);

    //! efface tout le texte
    void eraseText();

    //! affecte/desaffecte le focus
    inline void setFocus(bool isFocused_) {isFocused = isFocused_;}

    //! recupere le focus
    inline bool getFocus() const {return isFocused;}

    //! affecte une nouvelle position
    inline void setPosition(const Int2& parPosition) {pos = parPosition;}

    //! efface tout et change des parameters de l'EditText
    inline void setNbMaxCharsAndIsDigitOnly(int parNbMaxChars, bool parIsDigitOnly)
    {eraseText();nbMaxChars = parNbMaxChars;isDigitOnly = parIsDigitOnly;}

    //! recupere la position
    inline const Int2& getPosition() const {return pos;}

    //! destructeur
    ~EditText();

private:

    std::string text;
    int keypos;
    Int2 pos;
    char ctrl_space_char_1;
    char ctrl_space_char_2;
    bool isFocused;
    int nbMaxChars;
    int64_t timeCount;
    bool timeIsDisplayed;
    //const Font *font;
    bool isCrypted;
    bool isDigitOnly;
	std::string allowedCharacters;
	Color color;
};

//---------------------------------------------------------------------

#endif /*EDITTEXT_HPP*/
