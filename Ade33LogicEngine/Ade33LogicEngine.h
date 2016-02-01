/*
* Ade33LogicEngine.h
*
* Created: 05/01/2016 11:41:51
* Author: Paul Soulsby
*/


#ifndef __ADE33LOGICENGINE_H__
#define __ADE33LOGICENGINE_H__

#include <stdlib.h>
#include "Ade33LogicPatch.h"
#include "Ade33LogicPatchBase.h"
#include "Ade33LogicEngineBase.h"

class Ade33LogicEngine : public Ade33LogicPatchBase
{
	//variables
	public:
	static Ade33LogicEngine& getInstance()
	{
		static Ade33LogicEngine instance; // Guaranteed to be destroyed.
		// Instantiated on first use.
		return instance;
	}
	enum Func : unsigned char
	{
		FUNC_MODE,
		FUNC_A_SEL
	};
	enum Mode : unsigned char
	{
		MODE_AND,
		MODE_NAND,
		MODE_OR,
		MODE_NOR,
		MODE_XOR,
		MODE_XNOR
	};
	protected:
	private:
	static const unsigned char HIGH = 1;
	static const unsigned char LOW = 0;
	Ade33LogicPatch* patch_;
	Ade33LogicEngineBase* base_ = NULL;
	unsigned char inputA_[2] = {0};
	unsigned char inputB_ = 0;
	unsigned char output_ = LOW;
	//functions
	public:
	void construct(Ade33LogicEngineBase* base);
	const Ade33LogicPatch* getPatchPtr() const { return  patch_; }
	Ade33LogicPatch* getPatchPtr() { return patch_; }
	void setInputA(unsigned char index, unsigned char newValue);
	void setInputB(unsigned char newValue);
	void patchValueChanged(unsigned char func, unsigned char newValue);
	void refreshOutput();
	void incMode();
	protected:
	private:
	Ade33LogicEngine(Ade33LogicEngineBase* base);
	Ade33LogicEngine() {}
	Ade33LogicEngine( const Ade33LogicEngine &c );
	~Ade33LogicEngine();
	Ade33LogicEngine& operator=( const Ade33LogicEngine &c );

}; //Ade33LogicEngine

#endif //__ADE33LOGICENGINE_H__
