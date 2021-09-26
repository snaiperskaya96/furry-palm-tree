#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "LootableActor.generated.h"

UCLASS()
class SWARM_API ALootableActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ALootableActor();
protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	class USkeletalMeshComponent* Mesh = nullptr;
};
