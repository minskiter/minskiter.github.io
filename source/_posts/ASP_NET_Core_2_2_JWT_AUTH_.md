---
title: ASP.NET Core 2.2 JWT AUTH  
date: 2019-09-07 14:40:25
categories:
    - dotnet
tags:
    - dotnet
---


#### 为什么需要JWT

 - 服务端设置的cookie被浏览器所保护，无法跨域传输

 <!-- more  -->

#### DOTNET 对于JWT的支持

##### 头文件需要

``` cs
using Microsoft.IdentityModel.Tokens; //生成jwt token
using Microsoft.AspNetCore.Authentication.JwtBearer; // jwt auth 认证，需要用negut包安装
using Microsoft.AspNetCore.Authorization; // 认证
using System.Security.Claims; // 声明身份
using Microsoft.IdentityModel.Tokens; //Tokens
using System.Text; //编码
```

##### jwt token 生成

``` cs
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Microsoft.AspNetCore.Mvc;
using Microsoft.AspNetCore.Authentication;
using Microsoft.IdentityModel.Tokens;
using System.IdentityModel.Tokens.Jwt;
using System.Security.Claims;
using System.Text;

namespace authtest.Controllers
{
    [Route("[controller]/[action]")]
    public class JwtAuthController : Controller
    {
        public IActionResult Login()
        {
           var claims = new[]
            {
                new Claim(JwtRegisteredClaimNames.Sub,"test"),
                new Claim(JwtRegisteredClaimNames.Jti,Guid.NewGuid().ToString()),
                new Claim(ClaimTypes.Role,"user"),
                new Claim("org","true")
            };
            var key = new SymmetricSecurityKey(Encoding.UTF8.GetBytes("test6666666666666666666"));
            var creds = new SigningCredentials(key, SecurityAlgorithms.HmacSha256);
   
            var token = new JwtSecurityToken(null, null, claims, expires: DateTime.Now.AddMinutes(30), 
                signingCredentials: creds);
            var test = new JwtSecurityTokenHandler().WriteToken(token);
            return Json(test);

        }
    }
}
```

##### 自定义策略验证方式（Policy = "")

``` cs
/// <summary>
    /// 用户认证需求
    /// </summary>
    internal class UserRequirement : IAuthorizationRequirement
    {
        public string UserIdentity { get; private set; }

        public UserRequirement(string UserIdentityName)
        {
            UserIdentity = UserIdentityName;
        }
    }
    /// <summary>
    /// 使用的用户认证方法
    /// </summary>
    internal class UserAuthorizationHandler: AuthorizationHandler<UserRequirement>
    {
        protected override Task HandleRequirementAsync(AuthorizationHandlerContext context, UserRequirement requirement)
        {
            try
            {
                if (context.User.FindFirst(requirement.UserIdentity).Value.Equals("true"))
                {
                    context.Succeed(requirement);
                }
                else
                {
                    context.Fail();
                }                  
            }
            catch
            {
                context.Fail();
            }          
             return Task.CompletedTask;            
        }
    }
```

##### startUp.cs 服务里添加服务

``` cs
services.AddAuthentication(x =>
            {
                x.DefaultAuthenticateScheme = JwtBearerDefaults.AuthenticationScheme;
                x.DefaultChallengeScheme = JwtBearerDefaults.AuthenticationScheme;
            }).AddJwtBearer(option =>
            {
                option.TokenValidationParameters = new Microsoft.IdentityModel.Tokens.TokenValidationParameters
                {
                    IssuerSigningKey = new SymmetricSecurityKey(Encoding.UTF8.GetBytes("test6666666666666666666")),
                    ValidateAudience = false,
                     ValidateIssuer = false, 
                    ValidateIssuerSigningKey = true, // 签名校对
                    ValidateLifetime = true, // 时间校对
                };
            });

            services.AddAuthorization(options =>
            {
                options.AddPolicy("org", policy =>
                {
                    policy.Requirements.Add(new UserRequirement("org"));
                });
                options.AddPolicy("std", policy =>
                {
                    policy.Requirements.Add(new UserRequirement("std"));
                });
                options.AddPolicy("head", policy =>
                {
                    policy.Requirements.Add(new UserRequirement("head"));
                });
            });
```

##### 最后只要再需要权限验证的接口上添加特性

``` cs
[Authorize(Policy="{可以是org可以是std可以是head}")]
```


