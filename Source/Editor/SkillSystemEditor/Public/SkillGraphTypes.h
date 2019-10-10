// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SkillGraphTypes.generated.h"

USTRUCT()
struct SKILLSYSTEMEDITOR_API FSkillGraphNodeClassData
{
	GENERATED_USTRUCT_BODY()
public:
	FSkillGraphNodeClassData() {}
	FSkillGraphNodeClassData(UClass* InClass, const FString& InDeprecatedMessage);
	FSkillGraphNodeClassData(const FString& InAssetName, const FString& InGenratedClassPackage, const FString& InClassName, UClass* InClass);

	FString ToString()const;
	FString GetClassName()const;
	FText GetCategory()const;
	FString GetDisplayName()const;
	UClass* GetClass(bool bSilent = false);
	bool IsAbstract()const;

	FORCEINLINE bool IsBlueprint()const { return AssetName.Len() > 0; }
	FORCEINLINE bool IsDeprecated()const { return DeprecatedMessage.Len() > 0; }
	FORCEINLINE FString GetDeprecatedMessage()const { return DeprecatedMessage; }
	FORCEINLINE FString GetPackageName()const { return GeneratedClassPackage; }
public:
	uint32 bIsHidden : 1;
	uint32 bHideParent : 1;
private:
	TWeakObjectPtr<UClass> Class;

	UPROPERTY()
		FString AssetName;
	UPROPERTY()
		FString GeneratedClassPackage;
	UPROPERTY()
		FString ClassName;
	UPROPERTY()
		FText Category;
	FString DeprecatedMessage;
};


struct SKILLSYSTEMEDITOR_API FSkillGraphNodeClassNode
{
	FSkillGraphNodeClassData Data;
	FString ParentClassName;
	TSharedPtr<FSkillGraphNodeClassNode> ParentNode;
	TArray<TSharedPtr<FSkillGraphNodeClassNode>> SubNodes;
	void AddUniqueSubNode(TSharedPtr<FSkillGraphNodeClassNode> SubNode);

};

struct SKILLSYSTEMEDITOR_API FSkillGraphNodeClassHelper
{
	DECLARE_MULTICAST_DELEGATE(FOnPackageListUpdated);
	FSkillGraphNodeClassHelper(UClass* InRootClass);
	~FSkillGraphNodeClassHelper();

	void GatherClasses(const UClass* BaseClass, TArray<FSkillGraphNodeClassData>& AvailableClasses);
	static FString GetDeprecationMessage(const UClass* Class);

	void OnAssetAdded(const struct FAssetData& AssetData);
	void OnAssetRemoved(const struct FAssetData& AssetData);
	void InvalidateCache();
	void OnHotReload(bool bWasTriggeredAutomatically);

	static void AddUnknownClass(const FSkillGraphNodeClassData& ClassData);
	static bool IsClassKnown(const FSkillGraphNodeClassData& ClassData);
	static FOnPackageListUpdated OnPackageListUpdated;

	static int32 GetObservedBlueprintClassCount(UClass* BaseNativeClass);
	static void AddObservedBlueprintClasses(UClass* BaseNativeClass);
	void UpdateAvailableBlueprintClasses();

private:

	UClass* RootNodeClass;
	TSharedPtr<FSkillGraphNodeClassNode> RootNode;
	static TArray<FName> UnknownPackages;
	static TMap<UClass*, int32> BlueprintClassCount;

	TSharedPtr<FSkillGraphNodeClassNode> CreateClassDataNode(const struct FAssetData& AssetData);
	TSharedPtr<FSkillGraphNodeClassNode> FindBaseClassNode(TSharedPtr<FSkillGraphNodeClassNode> Node, const FString& ClassName);
	void FindAllSubClasses(TSharedPtr<FSkillGraphNodeClassNode> Node, TArray<FSkillGraphNodeClassData>& AvailableClasses);

	UClass* FindAssetClass(const FString& GeneratedClassPackage, const FString& AssetName);
	void BuildClassGraph();
	void AddClassGraphChildren(TSharedPtr<FSkillGraphNodeClassNode> Node, TArray<TSharedPtr<FSkillGraphNodeClassNode> >& NodeList);

	bool IsHidingClass(UClass* Class);
	bool IsHidingParentClass(UClass* Class);
	bool IsPackageSaved(FName PackageName);
};