using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.ComponentModel.DataAnnotations;

namespace UsersData.ViewModels.Authentication
{
    public class LoginVM
    {
        [Display(Name = "Użytkownik")]
        public string Username { get; set; }
        [Display(Name = "Hasło")]
        public string Password { get; set; }
    }
}