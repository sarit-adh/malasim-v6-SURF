# Treatment Strategies

This module implements various treatment strategy systems for malaria therapy administration, including multiple first-line therapies, cycling strategies, and adaptive approaches.

## Overview

The Treatment Strategies module provides:
- Strategy implementation
- Policy management
- Adaptation mechanisms
- Resistance handling
- Outcome optimization

## Components

### Core Strategy Types
- `IStrategy`: Base strategy interface
- `StrategyBuilder`: Strategy construction
- `CyclingStrategy`: Drug rotation
- `MFTStrategy`: Multiple First-line Therapy

### Advanced Strategies
- `AdaptiveCyclingStrategy`: Dynamic adaptation
- `NestedMFTStrategy`: Hierarchical MFT
- `DistrictMFTStrategy`: Location-specific
- `NovelDrugIntroductionStrategy`: New drug deployment
- `PublicPrivateStrategy`: Explicit public/private channels with a global share

### Multi-Location Strategies
- `MFTMultiLocationStrategy`: Regional MFT
- `NestedMFTMultiLocationStrategy`: Complex regional
- `PublicPrivateMultiLocationStrategy`: Explicit location-specific public/private channels
- `MFTRebalancingStrategy`: Distribution optimization

## Implementation

### Base Strategy
```cpp
class IStrategy {
    virtual Therapy* select_therapy(Person* person);
    virtual void update_strategy_state();
    virtual void initialize();
};
```

### Strategy Building
```cpp
class StrategyBuilder {
    void add_therapy(Therapy* therapy);
    void set_distribution(vector<double> distribution);
    IStrategy* build();
};
```

## Usage

### Basic Strategy
```cpp
// Initialize strategy
auto strategy = new MFTStrategy();
strategy->initialize();

// Select therapy
auto therapy = strategy->select_therapy(person);

// Update state
strategy->update_strategy_state();
```

### Complex Strategy
```cpp
// Build nested strategy
auto builder = new StrategyBuilder();
builder->add_therapy(therapy1);
builder->add_therapy(therapy2);
builder->set_distribution({0.6, 0.4});

auto strategy = builder->build();
```

## Key Features

### Strategy Types
- Single first-line therapy (SFT)
- Multiple first-line therapy (MFT)
- Cycling approaches
- Adaptive strategies
- Regional variations

### Policy Management
- Distribution control
- Therapy selection
- Adaptation rules
- Coverage patterns
- Outcome tracking

### Resistance Handling
- Resistance monitoring
- Strategy adaptation
- Drug rotation
- Efficacy tracking
- Risk mitigation

## Dependencies

### Core Components
- `Therapy`
- `Drug`
- `Person`
- `Population`

### Support Systems
- `Random`
- `Config`
- `Events`
- `Statistics`

## Notes

### Implementation Guidelines
- Validate strategy parameters
- Monitor resistance patterns
- Track treatment outcomes
- Update distributions
- Log strategy changes
- Handle edge cases
- Document decisions
- Test thoroughly

### Performance Considerations
- Optimize selection logic
- Cache common choices
- Monitor memory usage
- Profile critical paths
- Handle concurrency
- Validate inputs
- Clean up resources
- Update efficiently
