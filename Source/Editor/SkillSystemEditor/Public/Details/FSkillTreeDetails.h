// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Layout/Visibility.h"
#include "IDetailCustomization.h"

/**
 * 
 */
class SKILLSYSTEMEDITOR_API FSkillTreeDetails : public IDetailCustomization
{
public:
	static TSharedRef<IDetailCustomization> MakeInstance();

	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
	bool IsEditorOnlyObject() const;
private:
	void InitPropertyValues();

	class IPropertyUtilities* PropUtils;
};
