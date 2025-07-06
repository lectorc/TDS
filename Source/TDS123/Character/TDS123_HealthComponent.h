// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TDS123_HealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChange, float, Health, float, Damage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDead);


USTRUCT(BlueprintType)
struct FStatsParam
{
	GENERATED_BODY()
};




UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TDS123_API UTDS123_HealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category = "Health ")
	FOnHealthChange OnHealthChange;

	UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category = "Health ")
	FOnDead OnDead;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	float Health = 100.0f;

public:	
	UTDS123_HealthComponent();
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	UFUNCTION(BlueprintCallable, Category = "Health")
	float GetCurrentHealth();
	UFUNCTION(BlueprintCallable, Category = "Health")
	virtual void ReceiveDamage(float Damage);
	UFUNCTION(BlueprintNativeEvent)
	void DeadEvent();

		
};
