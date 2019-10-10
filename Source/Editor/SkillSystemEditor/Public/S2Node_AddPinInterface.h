// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "S2Node_AddPinInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class US2Node_AddPinInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SKILLSYSTEMEDITOR_API IS2Node_AddPinInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void AddInputPin() {}
	virtual bool CanAddPin()const  { return true; }
};
