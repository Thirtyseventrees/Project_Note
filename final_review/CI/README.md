- [Data Center](#data-center)
  - [In-house Data Center](#in-house-data-center)
  - [Co-Location Data Center](#co-location-data-center)
  - [Third-Party Managed Hosting](#third-party-managed-hosting)
- [Cloud Service Models](#cloud-service-models)
- [Warehouse-scale computers](#warehouse-scale-computers)
  - [Traditional data centers](#traditional-data-centers)
  - [Warehouse-scale computers (WSCs)](#warehouse-scale-computers-wscs)
    - [Architecture of WSCs](#architecture-of-wscs)
    - [Cloud Layering: Multi-Tenant Within a WSC](#cloud-layering-multi-tenant-within-a-wsc)
- [Multiple data centers](#multiple-data-centers)
  - [Hierarchical approach](#hierarchical-approach)
    - [Geographic Areas (GAs)](#geographic-areas-gas)
    - [Computing Regions (CRs)](#computing-regions-crs)
    - [Availability Zones (AZs)](#availability-zones-azs)
- [Availability of WSCs and DCs](#availability-of-wscs-and-dcs)
- [Server](#server)


# Data Center

A Data center is dedicated facility used to house an organization's computing resources and associated components.  
Typically include:
- Servers
- Storage system
- Networking equipment
- The infrastructure needed to power, connect, cool and physically secure them

Key characteristics:  
- **High Availability and Redundancy**:  
  Systems such as uninterruptible power supplies, backup generators, multiple network links, and redundant cooling to ensure minimal downtime

- **Environmental Controls**:  
  Heating, ventilation, and air conditioning (HVAC) systems, along with fire suppression, are used to **maintain optimal temperature, humidity, and safety conditions** for IT hardware.

- **Physicaal Security**:  
  Access control measures (ID badges, biometric scanners, CCTV) designed to protect against unauthorized entry

- **Scalablity**:  
  Data centers must accommodate growth over time, either by adding more racks/servers or by planning additional physical space.

要保证高可靠高冗余，环境控制（比如机房温度），物理安全（防止未经允许的人进入机房），可拓展性

## In-house Data Center

内部数据中心（自建数据中心）  
An in-house data center (onpremises data center) is a facility fully owned and operated by an organization on its own property.

前期投入：   
1. 要钱 (Upfront Investment 前期投资)
   - 数据中心所在的地方或者建筑
   - 能源和冷却系统
   - 硬件设备
2. 要人
   - 专业人员
   - 持续培训 (Ongoing Training)

**Advantages**:
1. Full Control  
   这个组织可以客制化硬件存放的位置，安全政策，计划维护而不用考虑外部的限制
2. Tangible Asset  
   公司倒闭了可以卖大楼
3. Customization for Specialized Workloads
   对于特定的用途可以定制化

**Disadvantages**:
1. High Initial Costs  
   要很多钱
2. Risk and Uncertainty  
   公司倒闭跑路了这个数据中心就没了
3. Complex Management  
   一个组织要负责这个数据中心日常运行的所有事项

## Co-Location Data Center

将你自己的设备本身托管给第三方，第三方提供能源冷却安保等服务

Cost Structure:  
- Monthly/Yearly Rental
- Hardware Ownership

**Advantages**:  
- Reduced Capital Expense:
  不要这么多钱
- Some Control
- Scalability

**Disadvantages**:  
- No Real Estata Asset  
  没有房地产资源
- Limited Customization  
  空调功率供电功率第三方说了算
- Potential Access Delays

## Third-Party Managed Hosting

所有东西都由第三方提供

# Cloud Service Models

<img src="./picture/image_1.png" alt="s" /> 

1. Infrastructure as a service (IaaS)
   - Definition:  
        The cloud provider offers virtual machines (VMs), networking and storage;  
        Clients manage operating systems, applications and data
    - Example:  
        AWS EC2, Microsoft Azure Virtual Machines, Google Compute Engine
    - Pros/Cons:  
        High flexibility;  
        But demands substantial system administration skills for OS and application management

2. Platform as a Service (Paas)
    - Definition:  
        Provides a managed environment that include OS, runtime, and databases;  
        Users only handly applications code and data
    - Example:  
        AWS Elastic Beanstalk, Microsoft Azure App Service, Google App Engine
    - Pros/Cons:   
        Streamlines application deployment;  
        But offers less control over underlying system settings and software versions.

3. Software as a Service (Saas)
    - Definition:
        The provider hosts the entire software application;  
        End user simply access it via internet.
    - Example:   
        Microsoft 365, Salesforce, Google Workspace
    - Pros/Cons:  
        Minimal administration overhead;  
        But highly standardized features with limited customization.

# Warehouse-scale computers

The trends toward server-sizde computing and widespread internet services created a new class of computing systems: warehouse-scale computer (WSCs):  
The massive scale of the software infrastructure, data repositories and hardware platform

## Traditional data centers

- typically host a large number relatively small- or medium- sized applications
- each applications is running on a dedicated hardware infrastructure that is de-coupled and protected from other systems in the same facility
- applications tend not to communicate each other

Those data centers host hardware and software for multiple organizational units or even different companies  

Each tenant managed a unique computing environment,  
resulting in a patchwork of hardware, operating systems

<img src="./picture/image_2.png" alt="s" width = 600/> 

通常的数据中心一般同时运行很多个种类不同，所属不同的任务

## Warehouse-scale computers (WSCs)

- homogeneity
- single-organization control
- cost efficiency

WSCs **belong to a single organization**， use a **relatively homogeneous hardware** and system software platform, and **share a common system management layer**

- WSCs run a smaller number of very large applications
- The common resource management infrastructure allows significant deployment flexibility

<img src="./picture/image_3.png" alt="s" width = 600/> 

### Architecture of WSCs

<img src="./picture/image_6.png" alt="s" width = 800/> 

### Cloud Layering: Multi-Tenant Within a WSC

WSC also can be partitioned into smaller logical segments that users rent as virtual machines or containers.  
In doing so, the data center’s uniform hardware underpinnings remain consistent, but portions of the compute, memory, and storage resources are allocated to individual clients. 

# Multiple data centers

- to reduce user latency
- improve serving throughput

A request is typically fully processed within one data center

## Hierarchical approach

### Geographic Areas (GAs)

The world is divided into Geographic Areas (GAs)
- Defined by Geo-political boundaries (or country borders)
- Determined mainly by data residency
- In each GA there are at least 2 computing regions

### Computing Regions (CRs)

- Customers see regions as the finer grain discretization of the infrastructure
- **Latency-defined** perimeter (**2ms latency for the round trip**)
- 100's of miles apart
- Too far for synchronous replication, but ok for disaster recovery

<img src="./picture/image_4.png" alt="s" width = 600/> 

### Availability Zones (AZs)

Availability Zones (AZs) are finer grain location within a single computing region.

- **Application-level synchronous** replication among AZs
- 3 AZs in CRs is minimum

<img src="./picture/image_5.png" alt="s" width = 600/> 

# Availability of WSCs and DCs

Services provided through WSCs (or DCs) must guarantee high availability, typically aiming for at least 99.99% **uptime** (i.e. one-hour downtime per year)

DC workloads must be designed to gracefully tolerate large numbers of component faults with little or no impact on service level performance and availability

# Server

- Servers are the main processing equipment
- Servers are hosted in individual shelves（架子）and are **the basic building blocks of DCs and WSCs**
- They are interconnected by hieraechies of networks and supported by the shared power and cooling infrastructure


