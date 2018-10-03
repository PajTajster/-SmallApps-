using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using UsersData.Models;
using System.ComponentModel.DataAnnotations;

namespace UsersData.ViewModels.User
{
    public class UserVM : Models.User
    {
        public UserVM()
        {

        }
        public UserVM (Models.User user)
        {
            ID = user.ID;
            FName = user.FName;
            LName = user.LName;
            Age = user.Age;
        }

        [Display(Name = "Nazwisko i Imię")]
        public string ShowName
        {
            get
            {
                return LName + "" + FName;
            }
        }

        public string HighLightAge
        {
            get
            {
                if(Age < 18)
                {
                    return "red";
                }
                return "black";
            }
        }
        public string BoldAge
        {
            get
            {
                if(Age < 18)
                {
                    return "bold";
                }
                return "normal";
            }
        }
    }
}