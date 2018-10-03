using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.ComponentModel.DataAnnotations;

namespace UsersData.Models
{
    public class User
    {
        [Key]
        [Display(Name = "Identyfikator")]
        [Required(ErrorMessage = " Proszę wprowadzić ID")]
        public int ID { get; set; }

        [Display(Name = "Imię")]
        [Required(ErrorMessage = " Proszę wprowadzić imię")]
        public string FName { get; set; }

        [Display(Name = "Nazwisko")]
        [Required(ErrorMessage = " Proszę wprowadzić nazwisko")]
        public string LName { get; set; }

        [Display(Name = "Wiek")]
        [Range(1,125,ErrorMessage = "Wiek musi być z zakresu 1-125")]
        [Required(ErrorMessage = " Proszę wprowadzić wiek")]
        public int Age { get; set; }
    }
}