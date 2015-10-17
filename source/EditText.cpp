/******************************************************************
un texte editable
Done by Francois Braud
******************************************************************/

#include "../include/Engine.h"
#include "../include/Utils.h"
#include "../include/EditText.h"

//---------------------------------------------------------------------
/**
    constructeur
    si font == NULL alors utilise une font par defaut
*/

EditText::EditText(
	bool isUrl,int parNbMaxChars, const Int2& parPosition,
	//const Font* parFont, 
	bool parIsCrypted, bool parIsDigitOnly,
	const std::string& parAllowedCharacters)
:
    text(""),
    keypos(0),
    pos(parPosition),
    ctrl_space_char_1(isUrl?'/':' '),
    ctrl_space_char_2(isUrl?'\\':' '),
    isFocused(true),
    nbMaxChars(parNbMaxChars),
    timeCount(0),
    //font(parFont),
    isCrypted(parIsCrypted),
    isDigitOnly(parIsDigitOnly),
	allowedCharacters(parAllowedCharacters),
	color(CoreUtils::colorWhite)
{
}

//---------------------------------------------------------------------
/**
    affiche l'EditText
    (a appeler a chaque appel de display)
*/

void EditText::draw(float fontScale)
{
	Scene2D& scene2D = Engine::instance().getScene2DMgr();

	//AssertRelease(font != NULL);
    if (isCrypted)
    {
        Int2 posRes = pos;
        for (size_t i = 0; i < text.size(); ++i)
        {
            //posx = font->drawText(color, Int2(posx,pos.y()),"*",fontScale).x();
			posRes = scene2D.drawText(NULL, "*", posRes, fontScale, color);
        }
    }
    else
    {
        //font->drawText(color, Int2(pos.x(), pos.y()),text.c_str(),fontScale);
		scene2D.drawText(NULL, text.c_str(), pos, fontScale, color);
    }

    if (isFocused)
    {
        int64_t tim = Utils::getMillisecond();

        if (tim > timeCount + 500)
        {
            timeCount = tim;
            timeIsDisplayed = !timeIsDisplayed;
        }

        if (timeIsDisplayed)
        {
            if (isCrypted)
            {
                //font->drawText(color, Int2(pos.x()+font->getWidth("*",-1,fontScale)*keypos-3,pos.y()),"|",fontScale);
				Int2 size = scene2D.getSizeText(NULL, "*", fontScale);
				scene2D.drawText(NULL, "|", Int2(pos.x()+size.width()*keypos-3,pos.y()), fontScale, color);
            }
            else
            {
				Int2 size = scene2D.getSizeText(NULL, text.c_str(), fontScale, keypos);
                //font->drawText(color, Int2(pos.x()+font->getWidth(text.c_str(),keypos,fontScale)-3,pos.y()),"|",fontScale);
				scene2D.drawText(NULL, "|", Int2(pos.x()+size.width()-3,pos.y()), fontScale, color);
            }
        }
    }
}

//---------------------------------------------------------------------
/**
    recupere les evenements clavier
    (c'est a dire met l'EditText au courant du fait qu'une touche a ete appuyee)
*/

void EditText::injectKeyEvent(unsigned char key)
{
    if (isFocused)
    {
		Scene2D& scene2D = Engine::instance().getScene2DMgr();

        if (key == (unsigned char)8) //backspace key : retour arriere
        {
            if (keypos>0)
            {
                text.erase(keypos-1,1);
                keypos--;
            }
        }
        else if (key == (unsigned char)127) //del
        {
            if (keypos < (int)text.size())
            {
                text.erase(keypos,1);
                //keypos;
            }
        }
        else if (key == (unsigned char)29) //cursor left : gauche
        {
            if (keypos > 0) keypos--;
        }
        else if (key == (unsigned char)28) //cursor right : droite
        {
            if (keypos < (int)text.length()) keypos++;
        }
        else if (key == (unsigned char)11) //home : debut
        {
            keypos = 0;
        }
        else
        {
            if (scene2D.isDrawableCharacter((char)key))
            {
                if (isDigitOnly)
                {
                    if (key>=48 && key<=57)
                        this->insertChar(key);
                }
                else
				{
					if (allowedCharacters != "")
					{
						if (allowedCharacters.find(key) != std::string::npos)
							this->insertChar(key);
					}
					else
						this->insertChar(key);
				}
            }
        }
    }
	//if (key != 27 && key != 10 && key != 13) // if key is not escape or return
	//	Engine::instance().getKeyboardManager()->getKeyNoRepeat(key);//disable key for other usage
}

//---------------------------------------------------------------------
/**
    insere un caractere (a la position courante du curseur)
*/

void EditText::insertChar(char c)
{
    if (nbMaxChars == -1 || (int)text.size() + 1 <= nbMaxChars)
    {
        text.insert(keypos, (char *)&c, 1);
        keypos++;
    }
}

//---------------------------------------------------------------------
/**
    insere une chaine de caracteres (a la fin)
*/

void EditText::insertStringAtEnd(const char *str)
{
    if (nbMaxChars==-1 || (int)text.size() + (int)strlen(str) <= nbMaxChars)
    {
        text += str;
        keypos = (int)text.size();
    }
}

//---------------------------------------------------------------------
/**
    efface tout le texte
*/

void EditText::eraseText()
{
    text.clear();
    keypos = 0;
}

//---------------------------------------------------------------------
/**
    destructeur
*/

EditText::~EditText()
{
	
}
